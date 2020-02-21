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

#include <dev/QTrafficGenerator.hpp>
#include <dev/QLatencyMeasurer.hpp>
#include <dev/QStatsCollector.hpp>
#include <dev/QFrameDetector.hpp>
#include <dev/QAbstractDevice.hpp>

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

	m_helloTimer = new QTimer(this);
	m_helloTimer->setInterval(MSG_HELLO_TIMEOUT);
	m_helloTimer->setSingleShot(false);

	connect(m_helloTimer, &QTimer::timeout, this, &QNetworkWorker::onHelloTimeout);
	connect(m_socket, &QTcpSocket::stateChanged, this, &QNetworkWorker::onNetworkStateChanged);
	connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &QNetworkWorker::onNetworkError);
	connect(m_socket, &QTcpSocket::readyRead, this, &QNetworkWorker::onReadyRead);
}

void QNetworkWorker::setDevices(const QVector<QAbstractDevice*> &devList)
{
	m_devices = devList;
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

void QNetworkWorker::setActiveBitstream(const QString &value)
{
	QByteArray args;
	appendAsBytes<quint16>(args, value.size());
	args.append(value.toUtf8());
	writeMessage(m_socket, MSG_ID_PROGRAM_PL, args);
}

void QNetworkWorker::getDeviceProperty(quint8 devID, quint32 propID)
{
	QByteArray args;
	appendAsBytes<quint8>(args, devID);
	appendAsBytes<quint16>(args, propID);
	writeMessage(m_socket, MSG_ID_GET_PROPERTY, args);
}

void QNetworkWorker::setDeviceProperty(quint8 devID, quint32 propID, const QByteArray &values)
{
	QByteArray args;
	appendAsBytes<quint8>(args, devID);
	appendAsBytes<quint16>(args, propID);
	args.append(values);
	writeMessage(m_socket, MSG_ID_SET_PROPERTY, args);
}

void QNetworkWorker::sendData(const QByteArray &data)
{
	m_socket->write(data);
}

void QNetworkWorker::startRun(bool exportResults)
{
	for(QAbstractDevice *dev : m_devices)
	{
		dev->resetMeasurement();
	}

	m_currentTime = 0;
	m_displayedTime = 0;

	if(exportResults)
	{
		QString timeStamp = QDateTime::currentDateTime().toString("yyyy_MM_dd HH_mm_ss");

		QDir measurementDir;
		measurementDir.mkpath("measurements/" + timeStamp);

		for(QAbstractDevice *dev : m_devices)
		{
			dev->enableLogging("measurements/" + timeStamp);
		}
	}

	emit timeChanged(0);
	writeMessage(m_socket, MSG_ID_RUN_START, QByteArray());
	setDeviceProperty(0xFF, PROP_ENABLE, QByteArray(1, '\1'));
}

void QNetworkWorker::stopRun()
{
	writeMessage(m_socket, MSG_ID_RUN_STOP, QByteArray());
}

void QNetworkWorker::onMessageReceived(quint16 id, const QByteArray &data)
{
	if(!(m_helloReceived ^ (id == MSG_ID_HELLO))) return;

	switch(id)
	{
		case MSG_ID_HELLO:
		{
			if(data.size() < 2) break;

			QStringList bitNames;

			for(int i = 0; i + 2 < data.size();)
			{
				quint16 nameLength = readAsNumber<quint16>(data, i);
				i += 2;

				if(!nameLength || i + nameLength - 1 >= data.size()) return;

				QByteArray name = data.mid(i, nameLength);
				i += nameLength;

				bitNames.append(QString::fromUtf8(name));
			}

			emit bitstreamsChanged(bitNames);

			m_helloTimer->stop();
			m_helloReceived = true;
			emit connectedChanged(ZBNT::Connected);
			break;
		}

		case MSG_ID_PROGRAM_PL:
		{
			if(data.size() < 3) break;

			quint8 success = readAsNumber<quint8>(data, 0);
			quint16 nameLength = readAsNumber<quint16>(data, 1);
			QString name = QString::fromUtf8(data.mid(3, nameLength));
			QList<BitstreamDevInfo> deviceList;

			int i = 3 + nameLength;
			while(i + 3 < data.size())
			{
				BitstreamDevInfo deviceInfo;
				deviceInfo.id = readAsNumber<quint8>(data, i);
				deviceInfo.type = DeviceType(readAsNumber<quint8>(data, i + 1));

				quint16 propsSize = readAsNumber<quint16>(data, i + 2);
				QByteArray propsData = data.mid(i + 4, propsSize);

				int j = 0;
				while(j + 3 < propsData.size())
				{
					PropertyID propID = PropertyID(readAsNumber<quint16>(propsData, j));
					quint16 valueSize = readAsNumber<quint16>(propsData, j + 2);
					QByteArray valueData = propsData.mid(j + 4, valueSize);

					deviceInfo.properties.append(qMakePair(propID, valueData));
					j += 4 + valueSize;
				}

				deviceList.append(deviceInfo);
				i += 4 + propsSize;
			}

			emit activeBitstreamChanged(success, name, deviceList);
			break;
		}

		case MSG_ID_RUN_START:
		{
			emit runningChanged(true);
			break;
		}

		case MSG_ID_RUN_STOP:
		{
			for(QAbstractDevice *dev : m_devices)
			{
				dev->disableLogging();
			}

			m_currentTime = 0;
			m_displayedTime = 0;

			emit runningChanged(false);
			break;
		}

		case MSG_ID_SET_PROPERTY:
		case MSG_ID_GET_PROPERTY:
		{
			if(data.size() < 4) break;

			quint8 devID = readAsNumber<quint8>(data, 0);
			quint16 propID = readAsNumber<quint16>(data, 1);
			quint8 success = readAsNumber<quint8>(data, 3);
			QByteArray value = data.mid(4);

			emit propertyChanged(success, devID, propID, value);
			break;
		}

		default:
		{
			if(id & MSG_ID_MEASUREMENT)
			{
				quint16 devID = id & ~MSG_ID_MEASUREMENT;

				if(devID < m_devices.size() && data.size() >= 8)
				{
					m_currentTime = readAsNumber<quint64>(data, 0);
					m_devices[devID]->receiveMeasurement(data);
				}
			}
		}
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
			m_helloTimer->stop();
			break;
		}

		case QAbstractSocket::ConnectingState:
		{
			emit connectedChanged(ZBNT::Connecting);
			break;
		}

		case QAbstractSocket::ConnectedState:
		{
			writeMessage(m_socket, MSG_ID_HELLO, QByteArray());
			m_helloReceived = false;
			m_helloTimer->start();
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

void QNetworkWorker::onHelloTimeout()
{
	emit connectionError("Server timeout, HELLO message not received");
	m_helloTimer->stop();
	m_socket->abort();
}

void QNetworkWorker::onReadyRead()
{
	handleIncomingData(m_socket->readAll());
}
