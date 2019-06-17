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

#include <zbnt.hpp>

#include <QDateTime>

#include <Utils.hpp>
#include <Messages.hpp>

ZBNT::ZBNT() : QObject(nullptr)
{
	m_tg0 = new QTrafficGenerator(this);
	m_tg1 = new QTrafficGenerator(this);
	m_lm0 = new QLatencyMeasurer(this);
	m_sc0 = new QStatsCollector(this);
	m_sc1 = new QStatsCollector(this);
	m_sc2 = new QStatsCollector(this);
	m_sc3 = new QStatsCollector(this);

	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	connect(m_socket, &QTcpSocket::stateChanged, this, &ZBNT::onNetworkStateChanged);
	connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &ZBNT::onNetworkError);
	connect(m_socket, &QTcpSocket::readyRead, this, &ZBNT::onReadyRead);
}

ZBNT::~ZBNT()
{ }

void ZBNT::sendSettings()
{
	// Send a stop message, this will reset the board peripherals

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_STOP);
	sendAsBytes<quint16>(m_socket, 0);

	// Traffic generators

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_TG_CFG);
	m_tg0->sendSettings(m_socket);

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_TG_CFG);
	m_tg1->sendSettings(m_socket);

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_TG_HEADERS);
	m_tg0->sendHeaders(m_socket);

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_TG_HEADERS);
	m_tg1->sendHeaders(m_socket);

	// Latency measurer

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_LM_CFG);
	m_lm0->sendSettings(m_socket);

	// Send start message

	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(m_socket, MSG_ID_START);
	sendAsBytes<quint16>(m_socket, 8);
	sendAsBytes(m_socket, m_runTime);
}

QString ZBNT::cyclesToTime(QString cycles)
{
	QString res;
	::cyclesToTime(cycles.toULongLong(), res);
	return res;
}

void ZBNT::autodetectBoardIP()
{
	// TODO
}

void ZBNT::connectToBoard()
{
	m_socket->connectToHost(m_ip, MSG_TCP_PORT);
}

void ZBNT::disconnectFromBoard()
{
	m_socket->disconnectFromHost();
}

void ZBNT::startRun()
{
	if(m_exportResults)
	{
		QString timeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

		m_sc0->enableLogging(QString("measurements_") + timeStamp + "_sc0.csv");
		m_sc1->enableLogging(QString("measurements_") + timeStamp + "_sc1.csv");
		m_sc2->enableLogging(QString("measurements_") + timeStamp + "_sc2.csv");
		m_sc3->enableLogging(QString("measurements_") + timeStamp + "_sc3.csv");
		m_lm0->enableLogging(QString("measurements_") + timeStamp + "_lm0.csv");
	}

	m_sc0->resetMeasurement();
	m_sc1->resetMeasurement();
	m_sc2->resetMeasurement();
	m_sc3->resetMeasurement();
	m_lm0->resetMeasurement();

	m_currentTime = 0;
	m_currentProgress = 0;
	emit measurementChanged();

	sendSettings();
	m_running = true;
	emit runningChanged();
}

void ZBNT::stopRun()
{
	m_socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, MSG_ID_STOP);
	sendAsBytes<uint16_t>(m_socket, 0);

	onRunEnd();
}

void ZBNT::onRunEnd()
{
	m_running = 0;

	m_sc0->disableLogging();
	m_sc1->disableLogging();
	m_sc2->disableLogging();
	m_sc3->disableLogging();
	m_lm0->disableLogging();

	emit measurementChanged();
	emit runningChanged();
}

QString ZBNT::runTime()
{
	return QString::number(m_runTime);
}

void ZBNT::setRunTime(QString time)
{
	m_runTime = time.toULongLong();
}

QString ZBNT::currentTime()
{
	return QString::number(m_currentTime);
}

void ZBNT::setCurrentTime(QString time)
{
	m_currentTime = time.toULongLong();
}

void ZBNT::onMessageReceived(quint8 id, const QByteArray &data)
{
	switch(id)
	{
		case MSG_ID_HELLO:
		{
			break;
		}

		case MSG_ID_DISCOVERY_RESP:
		{
			break;
		}

		case MSG_ID_MEASUREMENT_LM:
		{
			if(data.length() < 40) return;

			quint64 time = readAsNumber<quint64>(data, 0);

			if(time > m_currentTime)
			{
				m_currentTime = time;
				m_currentProgress = (m_currentTime / double(m_runTime)) * 2048;
			}

			m_lm0->receiveMeasurement(data);

			emit measurementChanged();
			break;
		}

		case MSG_ID_MEASUREMENT_SC:
		{
			if(data.length() < 57) return;

			quint8 idx = readAsNumber<quint8>(data, 0);
			quint64 time = readAsNumber<quint64>(data, 1);

			if(time > m_currentTime)
			{
				m_currentTime = time;
				m_currentProgress = (m_currentTime / double(m_runTime)) * 2048;
			}

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

			emit measurementChanged();
			break;
		}

		default: return;
	}
}

void ZBNT::onNetworkStateChanged(QAbstractSocket::SocketState state)
{
	switch(state)
	{
		case QAbstractSocket::UnconnectedState:
		{
			m_connected = Disconnected;
			break;
		}

		case QAbstractSocket::ConnectingState:
		{
			m_connected = Connecting;
			break;
		}

		case QAbstractSocket::ConnectedState:
		{
			m_connected = Connected;
			break;
		}

		default: { }
	}

	emit connectedChanged();
}

void ZBNT::onNetworkError(QAbstractSocket::SocketError error)
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

void ZBNT::onReadyRead()
{
	handleIncomingData(m_socket->readAll());
}
