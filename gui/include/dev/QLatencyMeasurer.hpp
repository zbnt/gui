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
#include <QMutex>
#include <QFile>

#include <dev/QAbstractDevice.hpp>

class QLatencyMeasurer : public QAbstractDevice
{
	Q_OBJECT

	Q_PROPERTY(quint8 enable MEMBER m_enable NOTIFY settingsChanged)
	Q_PROPERTY(QString frameSize MEMBER m_frameSize NOTIFY settingsChanged)
	Q_PROPERTY(QString period MEMBER m_period NOTIFY settingsChanged)
	Q_PROPERTY(QString timeout MEMBER m_timeout NOTIFY settingsChanged)

	Q_PROPERTY(QString numPingPongs READ numPingPongs NOTIFY measurementChanged)
	Q_PROPERTY(QString numLostPings READ numLostPings NOTIFY measurementChanged)
	Q_PROPERTY(QString numLostPongs READ numLostPongs NOTIFY measurementChanged)
	Q_PROPERTY(QString lastPing READ lastPing NOTIFY measurementChanged)
	Q_PROPERTY(QString lastPong READ lastPong NOTIFY measurementChanged)

	struct Measurement
	{
		quint64 time = 0;
		quint64 numPingPongs = 0;
		quint64 numLostPings = 0;
		quint64 numLostPongs = 0;
		quint64 lastPing = 0;
		quint64 lastPong = 0;
	};

public:
	QLatencyMeasurer(QObject *parent = nullptr);
	~QLatencyMeasurer();

	void enableLogging(const QString &path);
	void disableLogging();

	void updateDisplayedValues();

	void appendSettings(QByteArray &buffer);
	void receiveMeasurement(const QByteArray &measurement);
	void resetMeasurement();

public slots:
	QString numPingPongs();
	QString numLostPings();
	QString numLostPongs();
	QString lastPing();
	QString lastPong();

signals:
	void settingsChanged();
	void measurementChanged();

private:
	quint8 m_enable = 1;
	QString m_frameSize;
	QString m_period;
	QString m_timeout;

	Measurement m_currentValues, m_displayedValues;
	QMutex m_mutex;

	QFile m_logFile;
};
