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
#include <QVector>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include <dev/QTrafficGenerator.hpp>
#include <dev/QLatencyMeasurer.hpp>
#include <dev/QStatsCollector.hpp>
#include <dev/QFrameDetector.hpp>

#include <Messages.hpp>
#include <MessageReceiver.hpp>
#include <QDiscoveryClient.hpp>
#include <QNetworkWorker.hpp>

class ZBNT : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool running MEMBER m_running NOTIFY runningChanged)
	Q_PROPERTY(quint8 connected MEMBER m_connected NOTIFY connectedChanged)
	Q_PROPERTY(quint16 bitstreamID MEMBER m_bitstreamID NOTIFY settingsChanged)
	Q_PROPERTY(QString runTime READ runTime WRITE setRunTime NOTIFY settingsChanged)
	Q_PROPERTY(bool exportResults MEMBER m_exportResults NOTIFY settingsChanged)

	Q_PROPERTY(QVariantList deviceList MEMBER m_deviceList NOTIFY devicesChanged)
	Q_PROPERTY(QStringList bitstreamNames MEMBER m_bitstreamNames NOTIFY bitstreamsChanged)

	Q_PROPERTY(quint32 version READ version CONSTANT)
	Q_PROPERTY(QString versionStr READ versionStr CONSTANT)
	Q_PROPERTY(QString currentTime READ currentTime WRITE setCurrentTime NOTIFY timeChanged)
	Q_PROPERTY(quint32 currentProgress MEMBER m_currentProgress NOTIFY timeChanged)

public:
	ZBNT();
	~ZBNT();

	enum ConnectionStatus
	{
		Disconnected,
		Connecting,
		Connected
	};

	Q_ENUM(ConnectionStatus)

public slots:
	QByteArray arrayFromStr(const QString &data);
	QByteArray arrayFromNum(const QString &data, qint32 size);
	QString arrayToStr(const QByteArray &data, qint32 start, qint32 size);
	QVariant arrayToNum(const QByteArray &data, qint32 start, qint32 size);

	QString cyclesToTime(QString cycles);
	QString bytesToHumanReadable(QString bytes);
	QString estimateDataRate(quint64 size, quint64 delay);

	void scanDevices();
	void updateMeasurements();

	quint32 version();
	QString versionStr();

	QString runTime();
	void setRunTime(QString time);

	QString currentTime();
	void setCurrentTime(QString time);

	void onTimeChanged(quint64 time);
	void onRunningChanged(bool running);
	void onConnectedChanged(quint8 connected);
	void onConnectionError(QString error);
	void onBitstreamsChanged(QStringList names);
	void onActiveBitstreamChanged(quint8 success, const QString &name, const QList<BitstreamDevInfo> &devices);

signals:
	void timeChanged();
	void devicesChanged();
	void settingsChanged();
	void bitstreamsChanged();
	void bitstreamDevicesChanged(const QVector<QAbstractDevice*> &devices);

	void propertyChanged(quint8 success, quint8 devID, quint16 propID, const QByteArray &value);
	void activeBitstreamChanged(const QString &value);

	void runningChanged();
	void connectedChanged();
	void connectionError(QString error);

	void connectToBoard(const QString &ip, quint16 port);
	void disconnectFromBoard();

	void startRun(bool enableLog);
	void stopRun();
	void setActiveBitstream(const QString &value);
	void getDeviceProperty(quint8 devID, quint32 propID);
	void setDeviceProperty(quint8 devID, quint16 propID, const QByteArray &values);

private slots:
	void onDeviceDiscovered(const QByteArray &data);

private:
	QDiscoveryClient *m_discovery = nullptr;
	QNetworkWorker *m_netWorker = nullptr;
	QThread *m_netThread = nullptr;
	QTimer *m_updateTimer = nullptr;

	bool m_running = false;
	quint8 m_connected = 0;

	QVariantList m_deviceList;
	QStringList m_bitstreamNames;
	QVector<QAbstractDevice*> m_bitstreamDevices;

	quint64 m_runTime = 125000000ul;
	bool m_exportResults = true;
	quint16 m_bitstreamID = 0;

	quint64 m_currentTime = 0;
	quint32 m_currentProgress = 0;
};
