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

#include <QObject>
#include <QMutex>
#include <QFile>

#include <dev/QAbstractDevice.hpp>

class QLatencyMeasurer : public QAbstractDevice
{
	Q_OBJECT

	Q_PROPERTY(quint8 enable MEMBER m_enable NOTIFY settingsChanged)
	Q_PROPERTY(QString framePadding MEMBER m_framePadding NOTIFY settingsChanged)
	Q_PROPERTY(QString delay MEMBER m_delay NOTIFY settingsChanged)
	Q_PROPERTY(QString timeout MEMBER m_timeout NOTIFY settingsChanged)

	Q_PROPERTY(QString numPings READ numPings NOTIFY measurementChanged)
	Q_PROPERTY(QString pingTime READ pingTime NOTIFY measurementChanged)
	Q_PROPERTY(QString pongTime READ pongTime NOTIFY measurementChanged)
	Q_PROPERTY(QString numLostPings READ numLostPings NOTIFY measurementChanged)
	Q_PROPERTY(QString numLostPongs READ numLostPongs NOTIFY measurementChanged)

	struct Measurement
	{
		quint64 time = 0;
		quint64 numPings = 0;
		quint32 pingTime = 0;
		quint32 pongTime = 0;
		quint64 numLostPings = 0;
		quint64 numLostPongs = 0;
	};

public:
	QLatencyMeasurer(QObject *parent = nullptr);
	~QLatencyMeasurer();

	void loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props);

	void enableLogging(const QString &path);
	void disableLogging();

	void updateDisplayedValues();

	void receiveMeasurement(const QByteArray &measurement);
	void resetMeasurement();

public slots:
	QString description() const;
	QString settingsQml() const;
	QString statusQml() const;

	QString numPings();
	QString pingTime();
	QString pongTime();
	QString numLostPings();
	QString numLostPongs();

signals:
	void settingsChanged();
	void measurementChanged();

private:
	quint8 m_portA = 0;
	quint8 m_portB = 0;
	quint8 m_enable = 1;
	QString m_framePadding;
	QString m_delay;
	QString m_timeout;

	Measurement m_currentValues, m_displayedValues;
	QMutex m_mutex;

	QFile m_logFile;
};
