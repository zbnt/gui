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
	m_tg0 = new QTrafficGenerator(this);
	m_tg1 = new QTrafficGenerator(this);
	m_tg2 = new QTrafficGenerator(this);
	m_tg3 = new QTrafficGenerator(this);
	m_lm0 = new QLatencyMeasurer(this);
	m_sc0 = new QStatsCollector(this);
	m_sc1 = new QStatsCollector(this);
	m_sc2 = new QStatsCollector(this);
	m_sc3 = new QStatsCollector(this);
	m_fd0 = new QFrameDetector(this);

	m_tg0->setIndex(0);
	m_tg1->setIndex(1);
	m_tg2->setIndex(2);
	m_tg3->setIndex(3);

	m_discovery = new QDiscoveryClient(this);
	QTimer::singleShot(1000, this, &ZBNT::scanDevices);

	m_netThread = new QThread();
	m_netThread->start();

	m_netWorker = new QNetworkWorker();
	m_netWorker->setLatencyMeasurer(m_lm0);
	m_netWorker->setStatsCollectors(m_sc0, m_sc1, m_sc2, m_sc3);
	m_netWorker->setFrameDetector(m_fd0);
	m_netWorker->moveToThread(m_netThread);
	QTimer::singleShot(0, m_netWorker, &QNetworkWorker::startWork);

	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(200);
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
}

ZBNT::~ZBNT()
{ }

void ZBNT::sendSettings()
{
	QByteArray txData;

	// Send a stop message, this will reset the board peripherals

	txData.append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(&txData, MSG_ID_STOP);
	appendAsBytes<quint16>(&txData, 0);

	// Reprogram the board if needed

	txData.append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(&txData, MSG_ID_SET_BITSTREAM);
	appendAsBytes<quint16>(&txData, 1);
	appendAsBytes<quint8>(&txData, m_bitstreamID);

	// Traffic generators

	if(!m_streamMode)
	{
		m_tg0->appendSettings(&txData);
		m_tg1->appendSettings(&txData);
	}

	m_tg0->appendHeaders(&txData);
	m_tg1->appendHeaders(&txData);

	if(m_bitstreamID == QuadTGen)
	{
		if(!m_streamMode)
		{
			m_tg2->appendSettings(&txData);
			m_tg3->appendSettings(&txData);
		}

		m_tg2->appendHeaders(&txData);
		m_tg3->appendHeaders(&txData);
	}

	// Latency measurer

	if(m_bitstreamID == DualTGenLM && !m_streamMode)
	{
		m_lm0->appendSettings(&txData);
	}

	// Frame detector

	if(m_bitstreamID == DualTGenFD && !m_streamMode)
	{
		m_fd0->appendSettings(&txData);
		m_fd0->appendPatterns(&txData);
	}

	// Send start message

	if(!m_streamMode)
	{
		txData.append(MSG_MAGIC_IDENTIFIER, 4);
		appendAsBytes<quint8>(&txData, MSG_ID_START);
		appendAsBytes<quint16>(&txData, 12);
		appendAsBytes<quint64>(&txData, m_runTime);
		appendAsBytes<quint8>(&txData, m_enableSC0);
		appendAsBytes<quint8>(&txData, m_enableSC1);
		appendAsBytes<quint8>(&txData, m_enableSC2);
		appendAsBytes<quint8>(&txData, m_enableSC3);
	}
	else
	{
		txData.append(MSG_MAGIC_IDENTIFIER, 4);
		appendAsBytes<quint8>(&txData, MSG_ID_START_STREAM);
		appendAsBytes<quint16>(&txData, 2);
		appendAsBytes<quint16>(&txData, m_streamPeriod);
	}

	// Send request to network thread

	emit reqSendData(txData);
}

QString ZBNT::cyclesToTime(QString cycles)
{
	QString res;
	::cyclesToTime(cycles.toULongLong(), res);
	return res;
}

void ZBNT::connectToBoard()
{
	emit reqConnectToBoard(m_ip);
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
	txData.append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(&txData, MSG_ID_STOP);
	appendAsBytes<quint16>(&txData, 0);
	emit reqSendData(txData);

	m_running = false;
	emit reqStopRun();
}

void ZBNT::updateMeasurements()
{
	if(m_running)
	{
		m_lm0->updateDisplayedValues();
		m_sc0->updateDisplayedValues();
		m_sc1->updateDisplayedValues();
		m_sc2->updateDisplayedValues();
		m_sc3->updateDisplayedValues();
		m_fd0->updateDisplayedValues();
		m_netWorker->updateDisplayedValues();
	}
}

void ZBNT::scanDevices()
{
	m_deviceList.clear();
	emit devicesChanged();

	m_discovery->findDevices();
}

quint32 ZBNT::networkVersion()
{
	return MSG_VERSION;
}

QString ZBNT::runTime()
{
	return QString::number(m_runTime);
}

void ZBNT::setRunTime(QString time)
{
	m_runTime = time.toULongLong();
}

QString ZBNT::streamPeriod()
{
	return QString::number(m_streamPeriod);
}

void ZBNT::setStreamPeriod(QString period)
{
	m_streamPeriod = period.toUShort();
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

void ZBNT::onDeviceDiscovered(const QByteArray &data)
{
	quint32 version = readAsNumber<quint32>(data, 0);
	quint8 hostLen = readAsNumber<quint8>(data, 4);
	quint8 numIP4 = readAsNumber<quint8>(data, 5);
	quint8 numIP6 = readAsNumber<quint8>(data, 6);
	quint64 time = readAsNumber<quint64>(data, 7);

	if(time != m_discovery->scanTime()) return;

	QVariantMap device;
	device["version"] = version;
	device["hostname"] = QByteArray(data.constData() + 15, hostLen);

	int i = 15 + hostLen;

	for(quint8 j = 0; j < numIP4; ++j)
	{
		quint32 ip = readAsNumber<quint32>(data, i);
		QHostAddress addr(ip);

		device["ip"] = addr.toString();
		m_deviceList.append(device);

		i += 4;
	}

	for(quint8 j = 0; j < numIP6; ++j)
	{
		Q_IPV6ADDR ip;
		memcpy(ip.c, data.constData() + i, 16);
		QHostAddress addr(ip);

		device["ip"] = addr.toString();
		m_deviceList.append(device);

		i += 16;
	}

	emit devicesChanged();
}
