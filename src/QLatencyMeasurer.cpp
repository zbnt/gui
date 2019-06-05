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

#include <QLatencyMeasurer.hpp>

#include <QString>

#include <Utils.hpp>
#include <controller.h>

QLatencyMeasurer::QLatencyMeasurer(QObject *parent) : QObject(parent)
{ }

QLatencyMeasurer::~QLatencyMeasurer()
{ }

void QLatencyMeasurer::enableLogging(const QString &fileName)
{
	disableLogging();

	m_logFile.setFileName(fileName);
	m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

void QLatencyMeasurer::disableLogging()
{
	if(m_logFile.isOpen()) m_logFile.close();
}

void QLatencyMeasurer::sendSettings(QTcpSocket *socket)
{
	if(!socket) return;

	sendAsBytes<quint16>(socket, 13);

	sendAsBytes<quint8>(socket, m_enable);
	sendAsBytes<quint32>(socket, m_frameSize.toULong() - 30);
	sendAsBytes<quint32>(socket, m_period.toULong());
	sendAsBytes<quint32>(socket, m_timeout.toULong());
}

void QLatencyMeasurer::receiveMeasurement(const QByteArray &measurement)
{
	quint64 time = readAsNumber<quint64>(measurement, 0);

	m_lastPing = readAsNumber<quint32>(measurement, 8);
	m_lastPong = readAsNumber<quint32>(measurement, 12);

	m_numPingPongs = readAsNumber<quint64>(measurement, 16);
	m_numLostPings = readAsNumber<quint64>(measurement, 24);
	m_numLostPongs = readAsNumber<quint64>(measurement, 32);

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(time));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_lastPing));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_lastPong));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_numPingPongs));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_numLostPings));
		m_logFile.write(",");
		m_logFile.write(QByteArray::number(m_numLostPongs));
		m_logFile.write("\n");
	}

	emit measurementChanged();
}

void QLatencyMeasurer::resetMeasurement()
{
	m_lastPing = 0;
	m_lastPong = 0;

	m_numPingPongs = 0;
	m_numLostPings = 0;
	m_numLostPongs = 0;

	emit measurementChanged();
}

QString QLatencyMeasurer::numPingPongs()
{
	return QString::number(m_numPingPongs);
}

QString QLatencyMeasurer::numLostPings()
{
	return QString::number(m_numLostPings);
}

QString QLatencyMeasurer::numLostPongs()
{
	return QString::number(m_numLostPongs);
}

QString QLatencyMeasurer::lastPong()
{
	QString res;
	cyclesToTime(m_lastPong, res);
	return res;
}

QString QLatencyMeasurer::lastPing()
{
	QString res;
	cyclesToTime(m_lastPing, res);
	return res;
}
