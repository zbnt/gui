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

#include <zbnt.hpp>

#include <QTimer>
#include <QDateTime>

#include <Utils.hpp>
#include <Version.hpp>
#include <Messages.hpp>
#include <MessageUtils.hpp>

ZBNT::ZBNT() : QObject(nullptr)
{
	m_discovery = new QDiscoveryClient(this);
	QTimer::singleShot(1000, this, &ZBNT::scanDevices);

	m_netThread = new QThread();
	m_netThread->start();

	m_netWorker = new NetWorker();
	m_netWorker->moveToThread(m_netThread);
	QTimer::singleShot(0, m_netWorker, &NetWorker::startWork);

	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(250);
	m_updateTimer->setSingleShot(false);
	m_updateTimer->start();

	connect(m_updateTimer, &QTimer::timeout, this, &ZBNT::updateMeasurements);

	connect(m_discovery, &QDiscoveryClient::deviceDiscovered, this, &ZBNT::onDeviceDiscovered);
	connect(m_discovery, &QDiscoveryClient::discoveryTimeout, this, &ZBNT::onDiscoveryTimeout);

	connect(this, &ZBNT::bitstreamDevicesChanged, m_netWorker, &NetWorker::setDevices);
	connect(this, &ZBNT::connectTcp, m_netWorker, &NetWorker::connectTcp);
	connect(this, &ZBNT::connectLocal, m_netWorker, &NetWorker::connectLocal);
	connect(this, &ZBNT::disconnectFromBoard, m_netWorker, &NetWorker::disconnectFromBoard);
	connect(this, &ZBNT::setActiveBitstream, m_netWorker, &NetWorker::setActiveBitstream);
	connect(this, &ZBNT::setDeviceProperty, m_netWorker, &NetWorker::setDeviceProperty);
	connect(this, &ZBNT::getDeviceProperty, m_netWorker, &NetWorker::getDeviceProperty);
	connect(this, &ZBNT::getDevicePropertyWithArgs, m_netWorker, &NetWorker::getDevicePropertyWithArgs);
	connect(this, &ZBNT::startRun, m_netWorker, &NetWorker::startRun);
	connect(this, &ZBNT::stopRun, m_netWorker, &NetWorker::stopRun);

	connect(m_netWorker, &NetWorker::timeChanged, this, &ZBNT::onTimeChanged);
	connect(m_netWorker, &NetWorker::runningChanged, this, &ZBNT::onRunningChanged);
	connect(m_netWorker, &NetWorker::connectedChanged, this, &ZBNT::onConnectedChanged);
	connect(m_netWorker, &NetWorker::connectionError, this, &ZBNT::onConnectionError);
	connect(m_netWorker, &NetWorker::bitstreamsChanged, this, &ZBNT::onBitstreamsChanged);
	connect(m_netWorker, &NetWorker::activeBitstreamChanged, this, &ZBNT::onActiveBitstreamChanged);
	connect(m_netWorker, &NetWorker::propertyChanged, this, &ZBNT::propertyChanged);
}

ZBNT::~ZBNT()
{ }

QByteArray ZBNT::arrayConcat(const QByteArray &a, const QByteArray &b)
{
	return a + b;
}

QByteArray ZBNT::arrayFromStr(const QString &data)
{
	return data.toUtf8();
}

QByteArray ZBNT::arrayFromNum(const QString &data, qint32 size)
{
	QByteArray res;
	bool ok = false;
	quint64 value = data.toULongLong(&ok);

	if(ok)
	{
		switch(size)
		{
			case 1:
			{
				appendAsBytes<quint8>(res, value);
				break;
			}

			case 2:
			{
				appendAsBytes<quint16>(res, value);
				break;
			}

			case 4:
			{
				appendAsBytes<quint32>(res, value);
				break;
			}

			case 8:
			{
				appendAsBytes<quint64>(res, value);
				break;
			}
		}
	}

	return res;
}

QString ZBNT::arrayToStr(const QByteArray &data, qint32 start, qint32 size)
{
	return QString::fromUtf8(data.mid(start, size));
}

QVariant ZBNT::arrayToNum(const QByteArray &data, qint32 start, qint32 size)
{
	if(start >= 0 && start + size <= data.size())
	{
		switch(size)
		{
			case 1:
			{
				return QVariant(readAsNumber<quint8>(data, start));
			}

			case 2:
			{
				return QVariant(readAsNumber<quint16>(data, start));
			}

			case 4:
			{
				return QVariant(readAsNumber<quint32>(data, start));
			}

			case 8:
			{
				return QVariant(QString::number(readAsNumber<quint64>(data, start)));
			}
		}
	}

	return QVariant(0);
}

QString ZBNT::cyclesToTime(QString cycles)
{
	QString res;
	::cyclesToTime(cycles.toULongLong(), res);
	return res;
}

QString ZBNT::bytesToHumanReadable(QString bytes)
{
	QString res;
	::bytesToHumanReadable(bytes.toULongLong(), res);
	return res;
}

QString ZBNT::estimateDataRate(quint64 size, quint64 delay)
{
	QString res;
	::bitsToHumanReadable(size * 1000000000 / (8 + size + delay + 4), res, true);
	res.append("/s");
	return res;
}

void ZBNT::scanDevices()
{
	m_deviceList.clear();
	emit devicesChanged();

	m_discovery->findDevices();
}

void ZBNT::updateMeasurements()
{
	if(m_running)
	{
		for(QAbstractDevice *dev : m_bitstreamDevices)
		{
			dev->updateDisplayedValues();
		}

		m_netWorker->updateDisplayedValues();
	}
}

quint32 ZBNT::version()
{
	return ZBNT_VERSION_INT;
}

QString ZBNT::versionStr()
{
	return ZBNT_VERSION;
}

QString ZBNT::runTime()
{
	return QString::number(m_runTime);
}

void ZBNT::setRunTime(QString time)
{
	m_runTime = time.toULongLong();
	emit settingsChanged();
}

QString ZBNT::currentTime()
{
	return QString::number(m_currentTime);
}

void ZBNT::setCurrentTime(QString time)
{
	m_currentTime = time.toULongLong();
	emit timeChanged();
}

void ZBNT::onTimeChanged(quint64 time)
{
	m_currentTime = time;
	m_currentProgress = (m_currentTime / double(m_runTime)) * 2048;
	emit timeChanged();
}

void ZBNT::onRunningChanged(bool running)
{
	if(!running)
	{
		updateMeasurements();

		if(m_running)
		{
			m_currentTime = m_runTime;
			m_currentProgress = 2048;
			emit timeChanged();
		}
	}

	m_running = running;
	emit runningChanged();
}

void ZBNT::onConnectedChanged(quint8 connected)
{
	m_connected = connected;

	if(connected == ZBNT::Disconnected)
	{
		m_bitstreamNames.clear();
		emit bitstreamsChanged();
	}

	emit connectedChanged();
}

void ZBNT::onConnectionError(QString error)
{
	emit connectionError(error);
}

void ZBNT::onBitstreamsChanged(QStringList names)
{
	m_bitstreamNames = names;
	emit bitstreamsChanged();
}

void ZBNT::onActiveBitstreamChanged(quint8 success, const QString &name, const QList<BitstreamDevInfo> &devices)
{
	Q_UNUSED(success);

	auto it = std::find(m_bitstreamNames.cbegin(), m_bitstreamNames.cend(), name);

	if(it != m_bitstreamNames.cend())
	{
		for(QAbstractDevice *dev : m_bitstreamDevices)
		{
			dev->deleteLater();
		}

		m_bitstreamDevices.clear();

		for(const BitstreamDevInfo &devInfo : devices)
		{
			QAbstractDevice *dev = nullptr;

			switch(devInfo.type)
			{
				case DEV_FRAME_DETECTOR:
				{
					dev = new QFrameDetector(this);
					break;
				}

				case DEV_STATS_COLLECTOR:
				{
					dev = new QStatsCollector(this);
					break;
				}

				case DEV_LATENCY_MEASURER:
				{
					dev = new QLatencyMeasurer(this);
					break;
				}

				case DEV_TRAFFIC_GENERATOR:
				{
					dev = new QTrafficGenerator(this);
					break;
				}

				default: { }
			}

			if(dev)
			{
				dev->setID(devInfo.id);
				dev->loadInitialProperties(devInfo.properties);
				m_bitstreamDevices.append(dev);
			}
		}

		emit bitstreamDevicesChanged(m_bitstreamDevices);
		emit activeBitstreamChanged(name);
	}
}

void ZBNT::onDeviceDiscovered(const QHostAddress &addr, const QByteArray &data)
{
	quint64 validator = readAsNumber<quint64>(data, 0);
	quint32 version = readAsNumber<quint32>(data, 8);

	if(validator != m_discovery->validator()) return;
	if((version & 0xFF000000) != (ZBNT_VERSION_INT & 0xFF000000)) return;

	QString versionStr = QString("%1.%2.%3").arg(version >> 24).arg((version >> 16) & 0xFF).arg(version & 0xFFFF);
	QString versionPrerel = QString::fromUtf8(data.mid(12, 16));
	QString versionCommit = QString::fromUtf8(data.mid(28, 16));
	quint8 versionDirty = readAsNumber<quint8>(data, 44);

	if(versionPrerel.size())
	{
		versionStr += "-";
		versionStr += versionPrerel;
	}

	if(versionCommit.size())
	{
		versionStr += "+";
		versionStr += versionCommit;

		if(versionDirty)
		{
			versionStr += ".d";
		}
	}
	else if(versionDirty)
	{
		versionStr += "+d";
	}

	quint8 local = readAsNumber<quint8>(data, 45);
	quint16 port = 0;
	qint64 pid = -1;

	if(!local)
	{
		port = readAsNumber<quint16>(data, 46);
	}
	else
	{
		pid = readAsNumber<qint64>(data, 46);
	}

	QVariantMap device;
	device["version"] = version;
	device["versionstr"] = versionStr;
	device["name"] = QByteArray(data.constData() + 54, data.size() - 54);
	device["local"] = local;
	device["port"] = port;
	device["pid"] = pid;

	bool ok = false;
	quint32 ip4 = addr.toIPv4Address(&ok);

	if(ok)
	{
		device["ip"] = QHostAddress(ip4).toString();
		device["fullAddr"] = device["ip"].toString() + ":" + device["port"].toString();
	}
	else
	{
		device["ip"] = addr.toString();
		device["fullAddr"] = "[" + device["ip"].toString() + "]:" + device["port"].toString();
	}

	m_deviceList.append(device);
}

void ZBNT::onDiscoveryTimeout()
{
	std::sort(m_deviceList.begin(), m_deviceList.end(),
		[](const QVariant &a, const QVariant &b)
		{
			QVariantMap mapA = a.toMap();
			QVariantMap mapB = b.toMap();

			if(mapA["local"].toUInt() && !mapB["local"].toUInt())
			{
				return true;
			}

			if(mapA["name"].toString() < mapB["name"].toString())
			{
				return true;
			}

			if(mapA["fullAddr"].toString().startsWith("[") && !mapB["fullAddr"].toString().startsWith("["))
			{
				return true;
			}

			return false;
		}
	);

	emit devicesChanged();
	emit discoveryDone();
}
