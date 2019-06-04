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

class QStatsCollector : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString txBytes READ txBytes NOTIFY measurementChanged)
	Q_PROPERTY(QString txGood READ txGood NOTIFY measurementChanged)
	Q_PROPERTY(QString txBad READ txBad NOTIFY measurementChanged)
	Q_PROPERTY(QString rxBytes READ rxBytes NOTIFY measurementChanged)
	Q_PROPERTY(QString rxGood READ rxGood NOTIFY measurementChanged)
	Q_PROPERTY(QString rxBad READ rxBad NOTIFY measurementChanged)

public:
	QStatsCollector(QObject *parent = nullptr);
	~QStatsCollector();

	void receiveMeasurement(const QByteArray &measurement);

public slots:
	QString txBytes();
	QString txGood();
	QString txBad();
	QString rxBytes();
	QString rxGood();
	QString rxBad();

signals:
	void settingsChanged();
	void measurementChanged();

private:
	quint64 m_txBytes = 0;
	quint64 m_txGood = 0;
	quint64 m_txBad = 0;
	quint64 m_rxBytes = 0;
	quint64 m_rxGood = 0;
	quint64 m_rxBad = 0;
};
