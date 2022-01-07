/*
	zbnt/gui
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

#include <net/NetWorker.hpp>

#include <QDir>
#include <QDateTime>
#include <QProcess>

#include <dev/QTrafficGenerator.hpp>
#include <dev/QLatencyMeasurer.hpp>
#include <dev/QStatsCollector.hpp>
#include <dev/QFrameDetector.hpp>
#include <dev/QAbstractDevice.hpp>

#include <net/ZbntLocalClient.hpp>
#include <net/ZbntTcpClient.hpp>

#include <Zbnt.hpp>
#include <MessageUtils.hpp>

constexpr quint32 PCAP_SHB_BLOCKTYPE   = 0x0A0D0D0A;
constexpr quint32 PCAP_BYTEORDER_MAGIC = 0x1A2B3C4D;

NetWorker::NetWorker(QObject *parent)
	: QObject(parent)
{ }

NetWorker::~NetWorker()
{ }

void NetWorker::startWork()
{
	m_helloTimer = new QTimer(this);
	m_helloTimer->setInterval(MSG_HELLO_TIMEOUT);
	m_helloTimer->setSingleShot(false);

	connect(m_helloTimer, &QTimer::timeout, this, &NetWorker::onHelloTimeout);
}

void NetWorker::setDevices(const QVector<QAbstractDevice*> &devList)
{
	m_devices.clear();

	for(QAbstractDevice *d : devList)
	{
		m_devices.insert(d->id(), d);
	}
}

void NetWorker::updateDisplayedValues()
{
	QMutexLocker lock(&m_mutex);

	if(m_displayedTime < m_currentTime)
	{
		m_displayedTime = m_currentTime;
		emit timeChanged(m_displayedTime);
	}
}

void NetWorker::connectTcp(const QString &ip, quint16 port)
{
	if(m_client)
	{
		m_client->abortConnection();
		m_client->deleteLater();
	}

	m_client = new ZbntTcpClient(ip, port, this);

	connect(m_client, &ZbntClient::stateChanged, this, &NetWorker::onStateChanged);
	connect(m_client, &ZbntClient::connectionError, this, &NetWorker::onConnectionError);
	connect(m_client, &ZbntClient::messageReceived, this, &NetWorker::onMessageReceived);

	m_client->connectToServer();
}

void NetWorker::connectLocal(qint64 pid)
{
	if(m_client)
	{
		m_client->abortConnection();
		m_client->deleteLater();
	}

	m_client = new ZbntLocalClient(pid, this);

	connect(m_client, &ZbntClient::stateChanged, this, &NetWorker::onStateChanged);
	connect(m_client, &ZbntClient::connectionError, this, &NetWorker::onConnectionError);
	connect(m_client, &ZbntClient::messageReceived, this, &NetWorker::onMessageReceived);

	m_client->connectToServer();
}

void NetWorker::disconnectFromBoard()
{
	m_client->closeConnection();
}

void NetWorker::setActiveBitstream(const QString &value)
{
	QByteArray args;
	appendAsBytes<quint16>(args, value.size());
	args.append(value.toUtf8());
	m_client->writeMessage(MSG_ID_PROGRAM_PL, args);
}

void NetWorker::getDeviceProperty(quint8 devID, quint32 propID)
{
	QByteArray args;
	appendAsBytes<quint8>(args, devID);
	appendAsBytes<quint16>(args, propID);
	m_client->writeMessage(MSG_ID_GET_PROPERTY, args);
}

void NetWorker::getDevicePropertyWithArgs(quint8 devID, quint32 propID, const QByteArray &params)
{
	QByteArray args;
	appendAsBytes<quint8>(args, devID);
	appendAsBytes<quint16>(args, propID);
	args.append(params);
	m_client->writeMessage(MSG_ID_GET_PROPERTY, args);
}

void NetWorker::setDeviceProperty(quint8 devID, quint32 propID, const QByteArray &values)
{
	QByteArray args;
	appendAsBytes<quint8>(args, devID);
	appendAsBytes<quint16>(args, propID);
	args.append(values);
	m_client->writeMessage(MSG_ID_SET_PROPERTY, args);
}

void NetWorker::sendData(const QByteArray &data)
{
	m_client->write(data);
}

void NetWorker::startRun(bool exportResults, bool openWireshark)
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

		// Set up PCap

		std::shared_ptr<QIODevice> pcapOutput;

		if(!openWireshark)
		{
			QFile *logFile = new QFile();

			if(logFile)
			{
				logFile->setFileName("measurements/" + timeStamp + "/traffic.pcapng");
				logFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
			}

			pcapOutput.reset(logFile);
		}
		else
		{
			QProcess *wireshark = new QProcess();

			pcapOutput.reset(wireshark);

			if(wireshark)
			{
				std::shared_ptr<QIODevice> wiresharkProcess = pcapOutput;

				connect(wireshark, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
					[wiresharkProcess](int, QProcess::ExitStatus)
					{
						Q_UNUSED(wiresharkProcess);
					}
				);

				wireshark->start("wireshark", {"-k", "-i", "-"}, QIODevice::WriteOnly);
			}
		}

		if(pcapOutput)
		{
			writeAsBytes(pcapOutput, PCAP_SHB_BLOCKTYPE);
			writeAsBytes(pcapOutput, quint32(28));
			writeAsBytes(pcapOutput, PCAP_BYTEORDER_MAGIC);
			writeAsBytes(pcapOutput, quint16(1));
			writeAsBytes(pcapOutput, quint16(0));
			pcapOutput->write("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8);
			writeAsBytes(pcapOutput, quint32(28));
		}

		// Configure devices

		quint32 index = 0;

		for(QAbstractDevice *dev : m_devices)
		{
			dev->enableLogging("measurements/" + timeStamp);
			index += dev->setPcapOutput(pcapOutput, index);
		}
	}

	emit timeChanged(0);
	m_client->writeMessage(MSG_ID_RUN_START, QByteArray());
	setDeviceProperty(0xFF, PROP_ENABLE, QByteArray(1, '\1'));
}

void NetWorker::stopRun()
{
	m_client->writeMessage(MSG_ID_RUN_STOP, QByteArray());
}

void NetWorker::onStateChanged(ZbntClient::State state)
{
	switch(state)
	{
		case ZbntClient::Disconnected:
		{
			emit connectedChanged(Zbnt::Disconnected);
			emit runningChanged(false);
			m_helloTimer->stop();
			break;
		}

		case ZbntClient::Connecting:
		{
			emit connectedChanged(Zbnt::Connecting);
			break;
		}

		case ZbntClient::Connected:
		{
			m_helloReceived = false;
			m_client->writeMessage(MSG_ID_HELLO, QByteArray());
			m_helloTimer->start();
			break;
		}

		default: { }
	}
}

void NetWorker::onConnectionError(const char *error)
{
	emit connectionError(error);
}

void NetWorker::onMessageReceived(quint16 id, const QByteArray &data)
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
			emit connectedChanged(Zbnt::Connected);
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
				auto it = m_devices.constFind(devID);

				if(it != m_devices.constEnd() && data.size() >= 8)
				{
					m_currentTime = readAsNumber<quint64>(data, 0);
					it.value()->receiveMeasurement(data);
				}
			}
		}
	}
}

void NetWorker::onHelloTimeout()
{
	emit connectionError("Server timeout, HELLO message not received");
	m_client->abortConnection();
	m_helloTimer->stop();
}
