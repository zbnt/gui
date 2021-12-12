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

#include <dev/QLatencyMeasurer.hpp>

#include <QString>

#include <Utils.hpp>
#include <Messages.hpp>
#include <MessageUtils.hpp>

QLatencyMeasurer::QLatencyMeasurer(QObject *parent)
	: QAbstractDevice(parent)
{ }

QLatencyMeasurer::~QLatencyMeasurer()
{ }

void QLatencyMeasurer::loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props)
{
	for(auto prop : props)
	{
		switch(prop.first)
		{
			case PROP_PORTS:
			{
				if(prop.second.size() < 2) break;

				m_portA = readAsNumber<quint8>(prop.second, 0);
				m_portB = readAsNumber<quint8>(prop.second, 1);
				break;
			}

			default: { }
		}
	}
}

void QLatencyMeasurer::enableLogging(const QString &path)
{
	disableLogging();

	m_logFile.setFileName(path + QString("/eth%1_eth%2_latency.csv").arg(m_portA).arg(m_portB));
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

	m_currentValues.numPings = readAsNumber<quint64>(measurement, 8);
	m_currentValues.pingTime = readAsNumber<quint32>(measurement, 16);
	m_currentValues.pongTime = readAsNumber<quint32>(measurement, 20);

	m_currentValues.numLostPings = readAsNumber<quint64>(measurement, 24);
	m_currentValues.numLostPongs = readAsNumber<quint64>(measurement, 32);

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(m_currentValues.time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.numPings));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.pingTime));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.pongTime));
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

	m_currentValues.numPings = 0;
	m_currentValues.pingTime = 0;
	m_currentValues.pongTime = 0;

	m_currentValues.numLostPings = 0;
	m_currentValues.numLostPongs = 0;
}

QString QLatencyMeasurer::description() const
{
	return QString("Latency measurer (eth%1, eth%2)").arg(m_portA).arg(m_portB);
}

QString QLatencyMeasurer::settingsQml() const
{
	return QString("qrc:/qml/SettingsTabLM.qml");
}

QString QLatencyMeasurer::statusQml() const
{
	return QString("qrc:/qml/StatusTabLM.qml");
}

QString QLatencyMeasurer::numPings()
{
	return QString::number(m_currentValues.numPings);
}

QString QLatencyMeasurer::pingTime()
{
	QString res;
	cyclesToTime(m_currentValues.pingTime, res);
	return res;
}

QString QLatencyMeasurer::pongTime()
{
	QString res;
	cyclesToTime(m_currentValues.pongTime, res);
	return res;
}

QString QLatencyMeasurer::numLostPings()
{
	return QString::number(m_currentValues.numLostPings);
}

QString QLatencyMeasurer::numLostPongs()
{
	return QString::number(m_currentValues.numLostPongs);
}
