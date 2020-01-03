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

#include <QTimer>
#include <QDateTime>

#include <Utils.hpp>
#include <Messages.hpp>

ZBNT::ZBNT() : QObject(nullptr)
{
	m_discovery = new QDiscoveryClient(this);
	QTimer::singleShot(1000, this, &ZBNT::scanDevices);

	m_netThread = new QThread();
	m_netThread->start();

	m_netWorker = new QNetworkWorker();
	m_netWorker->moveToThread(m_netThread);
	QTimer::singleShot(0, m_netWorker, &QNetworkWorker::startWork);

	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(250);
	m_updateTimer->setSingleShot(false);
	m_updateTimer->start();

	connect(m_updateTimer, &QTimer::timeout, this, &ZBNT::updateMeasurements);

	connect(m_discovery, &QDiscoveryClient::deviceDiscovered, this, &ZBNT::onDeviceDiscovered);

	connect(this, &ZBNT::connectToBoard, m_netWorker, &QNetworkWorker::connectToBoard);
	connect(this, &ZBNT::disconnectFromBoard, m_netWorker, &QNetworkWorker::disconnectFromBoard);
	connect(this, &ZBNT::setActiveBitstream, m_netWorker, &QNetworkWorker::setActiveBitstream);
	connect(this, &ZBNT::setDeviceProperty, m_netWorker, &QNetworkWorker::setDeviceProperty);
	connect(this, &ZBNT::getDeviceProperty, m_netWorker, &QNetworkWorker::getDeviceProperty);

	connect(m_netWorker, &QNetworkWorker::timeChanged, this, &ZBNT::onTimeChanged);
	connect(m_netWorker, &QNetworkWorker::runningChanged, this, &ZBNT::onRunningChanged);
	connect(m_netWorker, &QNetworkWorker::connectedChanged, this, &ZBNT::onConnectedChanged);
	connect(m_netWorker, &QNetworkWorker::connectionError, this, &ZBNT::onConnectionError);
	connect(m_netWorker, &QNetworkWorker::bitstreamsChanged, this, &ZBNT::onBitstreamsChanged);
	connect(m_netWorker, &QNetworkWorker::activeBitstreamChanged, this, &ZBNT::activeBitstreamChanged);
	connect(m_netWorker, &QNetworkWorker::activeBitstreamChanged, this, &ZBNT::onActiveBitstreamChanged);
	connect(m_netWorker, &QNetworkWorker::propertyChanged, this, &ZBNT::propertyChanged);
}

ZBNT::~ZBNT()
{ }

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
	return QString::fromUtf8(data).mid(start, size);
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
		for(QAbstractDevice *dev : m_devices)
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

void ZBNT::onBitstreamsChanged(QStringList names, BitstreamDevListList devLists)
{
	m_bitstreamNames = names;
	m_devLists = devLists;
	emit bitstreamsChanged();
}

void ZBNT::onActiveBitstreamChanged(quint8 success, const QString &value)
{
	Q_UNUSED(success)

	auto it = std::find(m_bitstreamNames.cbegin(), m_bitstreamNames.cend(), value);

	if(it != m_bitstreamNames.cend())
	{
		int idx = it - m_bitstreamNames.cbegin();

		// TODO: Destroy previous device instances and create new ones
	}
}

void ZBNT::onDeviceDiscovered(const QByteArray &data)
{
	quint32 version = readAsNumber<quint32>(data, 0);
	quint64 time = readAsNumber<quint64>(data, 4);

	if(time != m_discovery->scanTime()) return;
	if((version & 0xFF000000) != (ZBNT_VERSION_INT & 0xFF000000)) return;

	QVariantMap device;
	device["version"] = version;
	device["versionstr"] = QString("%1.%2.%3").arg(version >> 24).arg((version >> 16) & 0xFF).arg(version & 0xFFFF);
	device["hostname"] = QByteArray(data.constData() + 36, data.size() - 36);
	device["port"] = readAsNumber<quint16>(data, 32);

	Q_IPV6ADDR ip6;
	memcpy(ip6.c, data.constData() + 16, 16);

	if(memcmp(ip6.c, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16))
	{
		device["ip"] = QHostAddress(ip6).toString();
		device["fullAddr"] = "[" + device["ip"].toString() + "]:" + device["port"].toString();
		m_deviceList.append(device);
	}

	quint32 ip4 = readAsNumber<quint32>(data, 12);

	if(ip4)
	{
		device["ip"] = QHostAddress(ip4).toString();
		device["fullAddr"] = device["ip"].toString() + ":" + device["port"].toString();
		m_deviceList.append(device);
	}

	emit devicesChanged();
}
