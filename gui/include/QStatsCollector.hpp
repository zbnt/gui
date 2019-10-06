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

class QStatsCollector : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString txBytes READ txBytes NOTIFY measurementChanged)
	Q_PROPERTY(QString txGood READ txGood NOTIFY measurementChanged)
	Q_PROPERTY(QString txBad READ txBad NOTIFY measurementChanged)
	Q_PROPERTY(QString txRate READ txRate NOTIFY measurementChanged)

	Q_PROPERTY(QString rxBytes READ rxBytes NOTIFY measurementChanged)
	Q_PROPERTY(QString rxGood READ rxGood NOTIFY measurementChanged)
	Q_PROPERTY(QString rxBad READ rxBad NOTIFY measurementChanged)
	Q_PROPERTY(QString rxRate READ rxRate NOTIFY measurementChanged)

	struct Measurement
	{
		quint64 time = 0;
		quint64 txBytes = 0;
		quint64 txGood = 0;
		quint64 txBad = 0;
		quint64 rxBytes = 0;
		quint64 rxGood = 0;
		quint64 rxBad = 0;
	};

public:
	QStatsCollector(QObject *parent = nullptr);
	~QStatsCollector();

	void enableLogging(const QString &fileName);
	void disableLogging();

	void updateDisplayedValues();

	void receiveMeasurement(const QByteArray &measurement);
	void resetMeasurement();

public slots:
	QString txBytes();
	QString txGood();
	QString txBad();
	QString txRate();

	QString rxBytes();
	QString rxGood();
	QString rxBad();
	QString rxRate();

signals:
	void settingsChanged();
	void measurementChanged();

private:
	Measurement m_currentValues, m_displayedValues;
	QMutex m_mutex;

	quint64 m_lastRateUpdate = 0;
	quint64 m_lastTxBytes = 0;
	quint64 m_lastRxBytes = 0;
	quint64 m_txRate = 0;
	quint64 m_rxRate = 0;

	QFile m_logFile;
};
