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

#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>

#include <QTrafficGenerator.hpp>
#include <QLatencyMeasurer.hpp>
#include <QStatsCollector.hpp>
#include <QFrameDetector.hpp>

#include <Messages.hpp>
#include <MessageReceiver.hpp>

class QNetworkWorker : public QObject, public MessageReceiver
{
	Q_OBJECT

public:
	QNetworkWorker(QObject *parent = nullptr);
	~QNetworkWorker();

public slots:
	void startWork();
	void setLatencyMeasurer(QLatencyMeasurer *lm);
	void setStatsCollectors(QStatsCollector *sc0, QStatsCollector *sc1, QStatsCollector *sc2, QStatsCollector *sc3);
	void setFrameDetector(QFrameDetector *fd);

	void updateDisplayedValues();

	void connectToBoard(const QString &ip, quint16 port);
	void disconnectFromBoard();

	void sendData(const QByteArray &data);
	void startRun(bool exportResults);
	void stopRun();

private slots:
	void onMessageReceived(quint16 id, const QByteArray &data);
	void onNetworkStateChanged(QAbstractSocket::SocketState state);
	void onNetworkError(QAbstractSocket::SocketError error);
	void onReadyRead();

signals:
	void timeChanged(quint64 time);
	void runningChanged(bool running);
	void connectedChanged(quint8 connected);
	void connectionError(QString error);

private:
	QTcpSocket *m_socket = nullptr;
	QMutex m_mutex;

	quint64 m_currentTime = 0;
	quint64 m_displayedTime = 0;

	QLatencyMeasurer *m_lm0 = nullptr;
	QStatsCollector *m_sc0 = nullptr;
	QStatsCollector *m_sc1 = nullptr;
	QStatsCollector *m_sc2 = nullptr;
	QStatsCollector *m_sc3 = nullptr;
	QFrameDetector *m_fd0 = nullptr;
};

