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
#include <QUrl>

#include <QTrafficGenerator.hpp>
#include <QLatencyMeasurer.hpp>
#include <QStatsCollector.hpp>

class ZBNT : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool running MEMBER m_running NOTIFY runningChanged)
	Q_PROPERTY(bool connected MEMBER m_connected NOTIFY connectedChanged)

	Q_PROPERTY(QString ip MEMBER m_ip NOTIFY settingsChanged)
	Q_PROPERTY(QString runTime READ runTime WRITE setRunTime NOTIFY settingsChanged)
	Q_PROPERTY(bool exportResults MEMBER m_exportResults NOTIFY settingsChanged)

	Q_PROPERTY(QString currentTime READ currentTime WRITE setCurrentTime NOTIFY measurementChanged)
	Q_PROPERTY(quint32 currentProgress MEMBER m_currentProgress NOTIFY measurementChanged)

	Q_PROPERTY(QTrafficGenerator *tg0 MEMBER m_tg0 CONSTANT)
	Q_PROPERTY(QTrafficGenerator *tg1 MEMBER m_tg1 CONSTANT)
	Q_PROPERTY(QLatencyMeasurer *lm0 MEMBER m_lm0 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc0 MEMBER m_sc0 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc1 MEMBER m_sc1 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc2 MEMBER m_sc2 CONSTANT)
	Q_PROPERTY(QStatsCollector *sc3 MEMBER m_sc3 CONSTANT)

public:
	ZBNT();
	~ZBNT();

public slots:
	QString cyclesToTime(QString cycles);

	void autodetectBoardIP();
	void connectToBoard();
	void disconnectFromBoard();

	void startRun();
	void stopRun();

	QString runTime();
	void setRunTime(QString time);

	QString currentTime();
	void setCurrentTime(QString time);

signals:
	void runningChanged();
	void settingsChanged();
	void connectedChanged();
	void measurementChanged();

private slots:
	void onConnected();
	void onDisconnected();
	void readSocket();

private:
	QTcpSocket *m_socket = nullptr;
	QByteArray m_readBuffer;

	bool m_running = false;
	bool m_connected = false;

	QString m_ip;
	quint64 m_runTime = 125000000ul;
	bool m_exportResults = true;

	quint64 m_currentTime = 0;
	quint32 m_currentProgress = 0;

	QTrafficGenerator *m_tg0 = nullptr;
	QTrafficGenerator *m_tg1 = nullptr;
	QLatencyMeasurer *m_lm0 = nullptr;
	QStatsCollector *m_sc0 = nullptr;
	QStatsCollector *m_sc1 = nullptr;
	QStatsCollector *m_sc2 = nullptr;
	QStatsCollector *m_sc3 = nullptr;
};
