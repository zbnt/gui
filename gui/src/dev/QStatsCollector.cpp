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

#include <dev/QStatsCollector.hpp>

#include <QString>

#include <Utils.hpp>
#include <Messages.hpp>

QStatsCollector::QStatsCollector(QObject *parent)
	: QAbstractDevice(parent)
{ }

QStatsCollector::~QStatsCollector()
{ }

void QStatsCollector::enableLogging(const QString &path)
{
	disableLogging();

	m_logFile.setFileName(path);
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

	if(m_currentValues.time - m_lastRateUpdate > 62500000)
	{
		m_txRate = (m_currentValues.txBytes - m_lastTxBytes) / (double(m_currentValues.time - m_lastRateUpdate) / 1000000000);
		m_rxRate = (m_currentValues.rxBytes - m_lastRxBytes) / (double(m_currentValues.time - m_lastRateUpdate) / 1000000000);

		m_lastRateUpdate = m_currentValues.time;
		m_lastTxBytes = m_currentValues.txBytes;
		m_lastRxBytes = m_currentValues.rxBytes;
	}

	emit measurementChanged();
}

void QStatsCollector::appendSettings(QByteArray &buffer)
{
	// TODO
	setDeviceProperty<quint8>(buffer, m_idx, PROP_ENABLE_LOG, 1);
	setDeviceProperty<quint32>(buffer, m_idx, PROP_SAMPLE_PERIOD, m_samplePeriod.toUInt());
	setDeviceProperty<quint8>(buffer, m_idx, PROP_ENABLE, 1);
}

void QStatsCollector::receiveMeasurement(const QByteArray &measurement)
{
	m_mutex.lock();

	m_currentValues.time = readAsNumber<quint64>(measurement, 0);

	m_currentValues.txBytes = readAsNumber<quint64>(measurement, 8);
	m_currentValues.txGood = readAsNumber<quint64>(measurement, 16);
	m_currentValues.txBad = readAsNumber<quint64>(measurement, 24);

	m_currentValues.rxBytes = readAsNumber<quint64>(measurement, 32);
	m_currentValues.rxGood = readAsNumber<quint64>(measurement, 40);
	m_currentValues.rxBad = readAsNumber<quint64>(measurement, 48);

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

QString QStatsCollector::txRate()
{
	QString res;
	bitsToHumanReadable(m_txRate, res, true);
	return res;
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

QString QStatsCollector::rxRate()
{
	QString res;
	bitsToHumanReadable(m_rxRate, res, true);
	return res;
}
