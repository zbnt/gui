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

#pragma once

#include <QHostAddress>
#include <QTcpSocket>
#include <QVector>
#include <QThread>
#include <QMutex>
#include <QTimer>

#include <dev/QTrafficGenerator.hpp>
#include <dev/QLatencyMeasurer.hpp>
#include <dev/QStatsCollector.hpp>
#include <dev/QFrameDetector.hpp>
#include <dev/QAbstractDevice.hpp>
#include <net/ZbntClient.hpp>

#include <Messages.hpp>
#include <MessageReceiver.hpp>

class NetWorker : public QObject
{
	Q_OBJECT

public:
	NetWorker(QObject *parent = nullptr);
	~NetWorker();

public slots:
	void startWork();
	void setDevices(const QVector<QAbstractDevice*> &devList);

	void updateDisplayedValues();

	void connectTcp(const QString &ip, quint16 port);
	void connectLocal(qint64 pid);
	void disconnectFromBoard();

	void setActiveBitstream(const QString &value);
	void getDeviceProperty(quint8 devID, quint32 propID);
	void getDevicePropertyWithArgs(quint8 devID, quint32 propID, const QByteArray &params);
	void setDeviceProperty(quint8 devID, quint32 propID, const QByteArray &values);

	void sendData(const QByteArray &data);
	void startRun(bool exportResults);
	void stopRun();

private slots:
	void onStateChanged(ZbntClient::State state);
	void onConnectionError(const char *error);
	void onMessageReceived(quint16 id, const QByteArray &data);
	void onHelloTimeout();

signals:
	void timeChanged(quint64 time);
	void runningChanged(bool running);
	void connectedChanged(quint8 connected);
	void connectionError(QString error);

	void bitstreamsChanged(QStringList names);
	void propertyChanged(quint8 success, quint8 devID, quint16 propID, const QByteArray &value);
	void activeBitstreamChanged(quint8 success, const QString &name, const QList<BitstreamDevInfo> &devices);

private:
	ZbntClient *m_client = nullptr;

	QMutex m_mutex;
	quint64 m_currentTime = 0;
	quint64 m_displayedTime = 0;
	QHash<quint8, QAbstractDevice*> m_devices;

	QTimer *m_helloTimer = nullptr;
	bool m_helloReceived = false;
};

