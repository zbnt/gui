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

#include <dev/QLatencyMeasurer.hpp>

#include <QString>

#include <Utils.hpp>
#include <Messages.hpp>

QLatencyMeasurer::QLatencyMeasurer(QObject *parent)
	: QAbstractDevice(parent)
{ }

QLatencyMeasurer::~QLatencyMeasurer()
{ }

void QLatencyMeasurer::enableLogging(const QString &path)
{
	disableLogging();

	m_logFile.setFileName(path);
	m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

void QLatencyMeasurer::disableLogging()
{
	if(m_logFile.isOpen()) m_logFile.close();
}

void QLatencyMeasurer::updateDisplayedValues()
{
	QMutexLocker lock(&m_mutex);

	memcpy(&m_displayedValues, &m_currentValues, sizeof(Measurement));

	emit measurementChanged();
}

void QLatencyMeasurer::receiveMeasurement(const QByteArray &measurement)
{
	if(measurement.size() < 40) return;

	m_mutex.lock();

	m_currentValues.time = readAsNumber<quint64>(measurement, 0);

	m_currentValues.lastPing = readAsNumber<quint32>(measurement, 8);
	m_currentValues.lastPong = readAsNumber<quint32>(measurement, 12);

	m_currentValues.numPingPongs = readAsNumber<quint64>(measurement, 16);
	m_currentValues.numLostPings = readAsNumber<quint64>(measurement, 24);
	m_currentValues.numLostPongs = readAsNumber<quint64>(measurement, 32);

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(m_currentValues.time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.lastPing));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.lastPong));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.numPingPongs));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.numLostPings));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.numLostPongs));
		m_logFile.putChar('\n');
	}
}

void QLatencyMeasurer::resetMeasurement()
{
	QMutexLocker lock(&m_mutex);

	m_currentValues.time = 0;

	m_currentValues.lastPing = 0;
	m_currentValues.lastPong = 0;

	m_currentValues.numPingPongs = 0;
	m_currentValues.numLostPings = 0;
	m_currentValues.numLostPongs = 0;
}

QString QLatencyMeasurer::description() const
{
	return QString("Latency measurer (eth%1, eth%2)").arg(m_ports & 0xFF).arg((m_ports >> 8) & 0xFF);
}

QString QLatencyMeasurer::settingsQml() const
{
	return QString("qrc:/qml/SettingsTabLM.qml");
}

QString QLatencyMeasurer::statusQml() const
{
	return QString("qrc:/qml/StatusTabLM.qml");
}

QString QLatencyMeasurer::numPingPongs()
{
	return QString::number(m_currentValues.numPingPongs);
}

QString QLatencyMeasurer::numLostPings()
{
	return QString::number(m_currentValues.numLostPings);
}

QString QLatencyMeasurer::numLostPongs()
{
	return QString::number(m_currentValues.numLostPongs);
}

QString QLatencyMeasurer::lastPong()
{
	QString res;
	cyclesToTime(m_currentValues.lastPong, res);
	return res;
}

QString QLatencyMeasurer::lastPing()
{
	QString res;
	cyclesToTime(m_currentValues.lastPing, res);
	return res;
}
