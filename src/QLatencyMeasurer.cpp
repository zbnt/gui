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

void QLatencyMeasurer::sendSettings(QTcpSocket *socket)
{
	if(!socket) return;

	sendAsBytes<quint16>(socket, 13);

	sendAsBytes<quint8>(socket, m_enable);
	sendAsBytes<quint32>(socket, m_frameSize.toULong() - 26);
	sendAsBytes<quint32>(socket, m_period.toULong());
	sendAsBytes<quint32>(socket, m_timeout.toULong());
}

void QLatencyMeasurer::receiveMeasurement(const QByteArray &measurement)
{
	m_lastE2E = readAsNumber<quint32>(measurement, 8);
	m_lastRT = readAsNumber<quint32>(measurement, 12);

	m_numPingPongs = readAsNumber<quint64>(measurement, 16);
	m_numLostPings = readAsNumber<quint64>(measurement, 24);
	m_numLostPongs = readAsNumber<quint64>(measurement, 32);

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

QString QLatencyMeasurer::lastRT()
{
	QString res;
	cyclesToTime(m_lastRT, res);
	return res;
}

QString QLatencyMeasurer::lastE2E()
{
	QString res;
	cyclesToTime(m_lastE2E, res);
	return res;
}

QString QLatencyMeasurer::avgRT()
{
	QString res;
	cyclesToTime(m_avgRT, res);
	return res;
}

QString QLatencyMeasurer::avgE2E()
{
	QString res;
	cyclesToTime(m_avgE2E, res);
	return res;
}
