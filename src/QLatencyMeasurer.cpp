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

QLatencyMeasurer::QLatencyMeasurer(QObject *parent) : QObject(parent)
{ }

QLatencyMeasurer::~QLatencyMeasurer()
{ }

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
