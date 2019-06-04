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

#include <QObject>
#include <QTcpSocket>

class QLatencyMeasurer : public QObject
{
	Q_OBJECT

	Q_PROPERTY(quint8 enable MEMBER m_enable NOTIFY settingsChanged)
	Q_PROPERTY(QString frameSize MEMBER m_frameSize NOTIFY settingsChanged)
	Q_PROPERTY(QString period MEMBER m_period NOTIFY settingsChanged)
	Q_PROPERTY(QString timeout MEMBER m_timeout NOTIFY settingsChanged)

	Q_PROPERTY(QString numPingPongs READ numPingPongs NOTIFY measurementChanged)
	Q_PROPERTY(QString numLostPings READ numLostPings NOTIFY measurementChanged)
	Q_PROPERTY(QString numLostPongs READ numLostPongs NOTIFY measurementChanged)
	Q_PROPERTY(QString lastRT READ lastRT NOTIFY measurementChanged)
	Q_PROPERTY(QString lastE2E READ lastE2E NOTIFY measurementChanged)
	Q_PROPERTY(QString avgRT READ avgRT NOTIFY measurementChanged)
	Q_PROPERTY(QString avgE2E READ avgE2E NOTIFY measurementChanged)

public:
	QLatencyMeasurer(QObject *parent = nullptr);
	~QLatencyMeasurer();

	void sendSettings(QTcpSocket *socket);
	void receiveMeasurement(const QByteArray &measurement);

public slots:
	QString numPingPongs();
	QString numLostPings();
	QString numLostPongs();
	QString lastRT();
	QString lastE2E();
	QString avgRT();
	QString avgE2E();

signals:
	void settingsChanged();
	void measurementChanged();

private:
	quint8 m_enable = 1;
	QString m_frameSize;
	QString m_period;
	QString m_timeout;

	quint64 m_numPingPongs = 0;
	quint64 m_numLostPings = 0;
	quint64 m_numLostPongs = 0;
	quint64 m_lastRT = 0;
	quint64 m_lastE2E = 0;
	quint64 m_avgRT = 0;
	quint64 m_avgE2E = 0;
};
