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

#pragma once

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include <QTrafficGenerator.hpp>
#include <QLatencyMeasurer.hpp>
#include <QStatsCollector.hpp>
#include <QFrameDetector.hpp>

#include <Messages.hpp>
#include <MessageReceiver.hpp>
#include <QDiscoveryClient.hpp>
#include <QNetworkWorker.hpp>

class ZBNT : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool running MEMBER m_running NOTIFY runningChanged)
	Q_PROPERTY(quint8 connected MEMBER m_connected NOTIFY connectedChanged)

	Q_PROPERTY(QString ip MEMBER m_ip NOTIFY settingsChanged)
	Q_PROPERTY(QVariantList deviceList MEMBER m_deviceList NOTIFY devicesChanged)
	Q_PROPERTY(quint32 networkVersion READ networkVersion CONSTANT)

	Q_PROPERTY(bool streamMode MEMBER m_streamMode NOTIFY settingsChanged)
	Q_PROPERTY(QString runTime READ runTime WRITE setRunTime NOTIFY settingsChanged)
	Q_PROPERTY(QString streamPeriod READ streamPeriod WRITE setStreamPeriod NOTIFY settingsChanged)
	Q_PROPERTY(bool exportResults MEMBER m_exportResults NOTIFY settingsChanged)
	Q_PROPERTY(bool enableSC0 MEMBER m_enableSC0 NOTIFY settingsChanged)
	Q_PROPERTY(bool enableSC1 MEMBER m_enableSC1 NOTIFY settingsChanged)
	Q_PROPERTY(bool enableSC2 MEMBER m_enableSC2 NOTIFY settingsChanged)
	Q_PROPERTY(bool enableSC3 MEMBER m_enableSC3 NOTIFY settingsChanged)
	Q_PROPERTY(quint8 bitstreamID MEMBER m_bitstreamID NOTIFY settingsChanged)

	Q_PROPERTY(QString currentTime READ currentTime WRITE setCurrentTime NOTIFY timeChanged)
	Q_PROPERTY(quint32 currentProgress MEMBER m_currentProgress NOTIFY timeChanged)

	Q_PROPERTY(QTrafficGenerator *tg0 MEMBER m_tg0 CONSTANT)
	Q_PROPERTY(QTrafficGenerator *tg1 MEMBER m_tg1 CONSTANT)
	Q_PROPERTY(QTrafficGenerator *tg2 MEMBER m_tg2 CONSTANT)
	Q_PROPERTY(QTrafficGenerator *tg3 MEMBER m_tg3 CONSTANT)
	Q_PROPERTY(QLatencyMeasurer *lm0 MEMBER m_lm0 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc0 MEMBER m_sc0 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc1 MEMBER m_sc1 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc2 MEMBER m_sc2 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc3 MEMBER m_sc3 CONSTANT)
	Q_PROPERTY(QFrameDetector *fd0 MEMBER m_fd0 CONSTANT)

public:
	ZBNT();
	~ZBNT();

	void sendSettings();

	enum ConnectionStatus
	{
		Disconnected,
		Connecting,
		Connected
	};

	enum BitstreamID
	{
		DualTGenLM = 1,
		DualTGenFD,
		QuadTGen
	};

	Q_ENUMS(ConnectionStatus)
	Q_ENUMS(BitstreamID)

public slots:
	QString cyclesToTime(QString cycles);
	QString bytesToHumanReadable(QString bytes);
	QString estimateDataRate(quint64 size, quint64 delay);

	void scanDevices();
	void connectToBoard();
	void disconnectFromBoard();

	void startRun();
	void stopRun();
	void updateMeasurements();

	quint32 networkVersion();

	QString runTime();
	void setRunTime(QString time);

	QString streamPeriod();
	void setStreamPeriod(QString period);

	QString currentTime();
	void setCurrentTime(QString time);

	void onTimeChanged(quint64 time);
	void onRunningChanged(bool running);
	void onConnectedChanged(quint8 connected);
	void onConnectionError(QString error);

signals:
	void timeChanged();
	void devicesChanged();
	void settingsChanged();

	void runningChanged();
	void connectedChanged();
	void connectionError(QString error);

	void reqConnectToBoard(const QString &ip);
	void reqDisconnectFromBoard();
	void reqSendData(const QByteArray &data);
	void reqStartRun(bool exportResults);
	void reqStopRun();

private slots:
	void onDeviceDiscovered(const QByteArray &data);

private:
	QDiscoveryClient *m_discovery = nullptr;
	QNetworkWorker *m_netWorker = nullptr;
	QThread *m_netThread = nullptr;
	QTimer *m_updateTimer = nullptr;

	bool m_running = false;
	quint8 m_connected = 0;

	QString m_ip;
	QVariantList m_deviceList;

	quint64 m_runTime = 125000000ul;
	quint16 m_streamPeriod = 1;
	bool m_streamMode = false;
	bool m_exportResults = true;
	bool m_enableSC0 = true;
	bool m_enableSC1 = true;
	bool m_enableSC2 = true;
	bool m_enableSC3 = true;
	quint8 m_bitstreamID = DualTGenLM;

	quint64 m_currentTime = 0;
	quint32 m_currentProgress = 0;

	QTrafficGenerator *m_tg0 = nullptr;
	QTrafficGenerator *m_tg1 = nullptr;
	QTrafficGenerator *m_tg2 = nullptr;
	QTrafficGenerator *m_tg3 = nullptr;
	QLatencyMeasurer *m_lm0 = nullptr;
	QStatsCollector *m_sc0 = nullptr;
	QStatsCollector *m_sc1 = nullptr;
	QStatsCollector *m_sc2 = nullptr;
	QStatsCollector *m_sc3 = nullptr;
	QFrameDetector *m_fd0 = nullptr;
};
