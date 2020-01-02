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

	connect(this, &ZBNT::reqConnectToBoard, m_netWorker, &QNetworkWorker::connectToBoard);
	connect(this, &ZBNT::reqDisconnectFromBoard, m_netWorker, &QNetworkWorker::disconnectFromBoard);
	connect(this, &ZBNT::reqSendData, m_netWorker, &QNetworkWorker::sendData);
	connect(this, &ZBNT::reqStartRun, m_netWorker, &QNetworkWorker::startRun);
	connect(this, &ZBNT::reqStopRun, m_netWorker, &QNetworkWorker::stopRun);

	connect(m_netWorker, &QNetworkWorker::timeChanged, this, &ZBNT::onTimeChanged);
	connect(m_netWorker, &QNetworkWorker::runningChanged, this, &ZBNT::onRunningChanged);
	connect(m_netWorker, &QNetworkWorker::connectedChanged, this, &ZBNT::onConnectedChanged);
	connect(m_netWorker, &QNetworkWorker::connectionError, this, &ZBNT::onConnectionError);
	connect(m_netWorker, &QNetworkWorker::bitstreamsChanged, this, &ZBNT::onBitstreamsChanged);
}

ZBNT::~ZBNT()
{ }

void ZBNT::sendSettings()
{
	QByteArray txData;

	// Send start message

	setDeviceProperty(txData, 0xFF, PROP_TIMER_LIMIT, m_runTime);
	setDeviceProperty(txData, 0xFF, PROP_ENABLE, 1);

	// Send request to network thread

	emit reqSendData(txData);
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

void ZBNT::connectToBoard()
{
	emit reqConnectToBoard(m_ip, m_port);
}

void ZBNT::disconnectFromBoard()
{
	emit reqDisconnectFromBoard();
}

void ZBNT::startRun()
{
	emit reqStartRun(m_exportResults);
	sendSettings();
}

void ZBNT::stopRun()
{
	QByteArray txData;
	setDeviceProperty(txData, 0xFF, PROP_ENABLE, 0);
	emit reqSendData(txData);

	m_running = false;
	emit reqStopRun();
}

void ZBNT::updateMeasurements()
{
	if(m_running)
	{
		// TODO

		m_netWorker->updateDisplayedValues();
	}
}

void ZBNT::scanDevices()
{
	m_deviceList.clear();
	emit devicesChanged();

	m_discovery->findDevices();
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
}

QString ZBNT::currentTime()
{
	return QString::number(m_currentTime);
}

void ZBNT::setCurrentTime(QString time)
{
	m_currentTime = time.toULongLong();
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
