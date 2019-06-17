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

#include <QStatsCollector.hpp>

#include <QString>

#include <Utils.hpp>

QStatsCollector::QStatsCollector(QObject *parent) : QObject(parent)
{ }

QStatsCollector::~QStatsCollector()
{ }

void QStatsCollector::enableLogging(const QString &fileName)
{
	disableLogging();

	m_logFile.setFileName(fileName);
	m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

void QStatsCollector::disableLogging()
{
	if(m_logFile.isOpen()) m_logFile.close();
}

void QStatsCollector::receiveMeasurement(const QByteArray &measurement)
{
	quint64 time = readAsNumber<quint64>(measurement, 1);

	m_txBytes = readAsNumber<quint64>(measurement, 9);
	m_txGood = readAsNumber<quint64>(measurement, 17);
	m_txBad = readAsNumber<quint64>(measurement, 25);

	m_rxBytes = readAsNumber<quint64>(measurement, 33);
	m_rxGood = readAsNumber<quint64>(measurement, 41);
	m_rxBad = readAsNumber<quint64>(measurement, 49);

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(time));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_txBytes));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_txGood));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_txBad));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_rxBytes));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_rxGood));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_rxBad));
		m_logFile.write("\n");
	}

	emit measurementChanged();
}

void QStatsCollector::resetMeasurement()
{
	m_txBytes = 0;
	m_txGood = 0;
	m_txBad = 0;

	m_rxBytes = 0;
	m_rxGood = 0;
	m_rxBad = 0;

	emit measurementChanged();
}

QString QStatsCollector::txBytes()
{
	return QString::number(m_txBytes);
}

QString QStatsCollector::txGood()
{
	return QString::number(m_txGood);
}

QString QStatsCollector::txBad()
{
	return QString::number(m_txBad);
}

QString QStatsCollector::rxBytes()
{
	return QString::number(m_rxBytes);
}

QString QStatsCollector::rxGood()
{
	return QString::number(m_rxGood);
}

QString QStatsCollector::rxBad()
{
	return QString::number(m_rxBad);
}
