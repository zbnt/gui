/*
	zbnt_gui
	Copyright (C) 2019 Oscar R.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <QNetworkWorker.hpp>

#include <QDir>
#include <QDateTime>

#include <QTrafficGenerator.hpp>
#include <QLatencyMeasurer.hpp>
#include <QStatsCollector.hpp>
#include <QFrameDetector.hpp>

#include <zbnt.hpp>
#include <Utils.hpp>

QNetworkWorker::QNetworkWorker(QObject *parent) : QObject(parent), MessageReceiver()
{ }

QNetworkWorker::~QNetworkWorker()
{ }

void QNetworkWorker::startWork()
{
	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	connect(m_socket, &QTcpSocket::stateChanged, this, &QNetworkWorker::onNetworkStateChanged);
	connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &QNetworkWorker::onNetworkError);
	connect(m_socket, &QTcpSocket::readyRead, this, &QNetworkWorker::onReadyRead);
}

void QNetworkWorker::setLatencyMeasurer(QLatencyMeasurer *lm)
{
	m_lm0 = lm;
}

void QNetworkWorker::setStatsCollectors(QStatsCollector *sc0, QStatsCollector *sc1, QStatsCollector *sc2, QStatsCollector *sc3)
{
	m_sc0 = sc0;
	m_sc1 = sc1;
	m_sc2 = sc2;
	m_sc3 = sc3;
}

void QNetworkWorker::setFrameDetector(QFrameDetector *fd)
{
	m_fd0 = fd;
}

void QNetworkWorker::updateDisplayedValues()
{
	QMutexLocker lock(&m_mutex);

	if(m_displayedTime < m_currentTime)
	{
		m_displayedTime = m_currentTime;
		emit timeChanged(m_displayedTime);
	}
}

void QNetworkWorker::connectToBoard(const QString &ip, quint16 port)
{
	m_socket->connectToHost(ip, port);
}

void QNetworkWorker::disconnectFromBoard()
{
	m_socket->disconnectFromHost();
}

void QNetworkWorker::sendData(const QByteArray &data)
{
	m_socket->write(data);
}

void QNetworkWorker::startRun(bool exportResults)
{
	if(exportResults)
	{
		QString timeStamp = QDateTime::currentDateTime().toString("yyyy_MM_dd HH_mm_ss");

		QDir measurementDir;
		measurementDir.mkpath("measurements/" + timeStamp);

		m_sc0->enableLogging("measurements/" + timeStamp + "/sc0.csv");
		m_sc1->enableLogging("measurements/" + timeStamp + "/sc1.csv");
		m_sc2->enableLogging("measurements/" + timeStamp + "/sc2.csv");
		m_sc3->enableLogging("measurements/" + timeStamp + "/sc3.csv");
		m_lm0->enableLogging("measurements/" + timeStamp + "/lm0.csv");
		m_fd0->enableLogging("measurements/" + timeStamp + "/fd0.csv");
	}

	m_sc0->resetMeasurement();
	m_sc1->resetMeasurement();
	m_sc2->resetMeasurement();
	m_sc3->resetMeasurement();
	m_lm0->resetMeasurement();

	m_currentTime = 0;
	m_displayedTime = 0;

	emit timeChanged(0);
	emit runningChanged(true);
}

void QNetworkWorker::stopRun()
{
	m_sc0->disableLogging();
	m_sc1->disableLogging();
	m_sc2->disableLogging();
	m_sc3->disableLogging();
	m_lm0->disableLogging();
	m_fd0->disableLogging();

	m_currentTime = 0;
	m_displayedTime = 0;

	emit runningChanged(false);
}

void QNetworkWorker::onMessageReceived(quint8 id, const QByteArray &data)
{
	switch(id)
	{
		case MSG_ID_DONE:
		{
			stopRun();
			break;
		}

		case MSG_ID_MEASUREMENT_LM:
		{
			if(data.length() < 40) return;

			m_currentTime = readAsNumber<quint64>(data, 0);

			m_lm0->receiveMeasurement(data);
			break;
		}

		case MSG_ID_MEASUREMENT_FD:
		{
			if(data.length() < 28) return;

			m_currentTime = readAsNumber<quint64>(data, 0);

			m_fd0->receiveMeasurement(data);
			break;
		}

		case MSG_ID_MEASUREMENT_SC:
		{
			if(data.length() < 57) return;

			quint8 idx = readAsNumber<quint8>(data, 0);
			m_currentTime = readAsNumber<quint64>(data, 1);

			switch(idx)
			{
				case 0:
				{
					m_sc0->receiveMeasurement(data);
					break;
				}

				case 1:
				{
					m_sc1->receiveMeasurement(data);
					break;
				}

				case 2:
				{
					m_sc2->receiveMeasurement(data);
					break;
				}

				case 3:
				{
					m_sc3->receiveMeasurement(data);
					break;
				}
			}

			break;
		}

		default: return;
	}
}

void QNetworkWorker::onNetworkStateChanged(QAbstractSocket::SocketState state)
{
	switch(state)
	{
		case QAbstractSocket::UnconnectedState:
		{
			emit connectedChanged(ZBNT::Disconnected);
			emit runningChanged(false);
			break;
		}

		case QAbstractSocket::ConnectingState:
		{
			emit connectedChanged(ZBNT::Connecting);
			break;
		}

		case QAbstractSocket::ConnectedState:
		{
			emit connectedChanged(ZBNT::Connected);
			break;
		}

		default: { }
	}
}

void QNetworkWorker::onNetworkError(QAbstractSocket::SocketError error)
{
	switch(error)
	{
		case QAbstractSocket::NetworkError:
		{
			emit connectionError("Unreachable address");
			break;
		}

		case QAbstractSocket::SocketTimeoutError:
		{
			emit connectionError("Timeout");
			break;
		}

		case QAbstractSocket::ConnectionRefusedError:
		{
			emit connectionError("Connection refused");
			break;
		}

		default:
		{
			emit connectionError("Unknown error");
		}
	}
}

void QNetworkWorker::onReadyRead()
{
	handleIncomingData(m_socket->readAll());
}
