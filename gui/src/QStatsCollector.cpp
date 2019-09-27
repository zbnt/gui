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

void QStatsCollector::updateDisplayedValues()
{
	QMutexLocker lock(&m_mutex);

	memcpy(&m_displayedValues, &m_currentValues, sizeof(Measurement));

	emit measurementChanged();
}

void QStatsCollector::receiveMeasurement(const QByteArray &measurement)
{
	m_mutex.lock();

	m_currentValues.time = readAsNumber<quint64>(measurement, 1);

	m_currentValues.txBytes = readAsNumber<quint64>(measurement, 9);
	m_currentValues.txGood = readAsNumber<quint64>(measurement, 17);
	m_currentValues.txBad = readAsNumber<quint64>(measurement, 25);

	m_currentValues.rxBytes = readAsNumber<quint64>(measurement, 33);
	m_currentValues.rxGood = readAsNumber<quint64>(measurement, 41);
	m_currentValues.rxBad = readAsNumber<quint64>(measurement, 49);

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(m_currentValues.time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.txBytes));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.txGood));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.txBad));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.rxBytes));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.rxGood));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.rxBad));
		m_logFile.putChar('\n');
	}
}

void QStatsCollector::resetMeasurement()
{
	QMutexLocker lock(&m_mutex);

	m_currentValues.time = 0;

	m_currentValues.txBytes = 0;
	m_currentValues.txGood = 0;
	m_currentValues.txBad = 0;

	m_currentValues.rxBytes = 0;
	m_currentValues.rxGood = 0;
	m_currentValues.rxBad = 0;
}

QString QStatsCollector::txBytes()
{
	return QString::number(m_displayedValues.txBytes);
}

QString QStatsCollector::txGood()
{
	return QString::number(m_displayedValues.txGood);
}

QString QStatsCollector::txBad()
{
	return QString::number(m_displayedValues.txBad);
}

QString QStatsCollector::rxBytes()
{
	return QString::number(m_displayedValues.rxBytes);
}

QString QStatsCollector::rxGood()
{
	return QString::number(m_displayedValues.rxGood);
}

QString QStatsCollector::rxBad()
{
	return QString::number(m_displayedValues.rxBad);
}