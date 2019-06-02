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

#include <zbnt.hpp>

#include <vector>

#include <Utils.hpp>

ZBNT::ZBNT() : QObject(nullptr)
{
	m_socket = new QTcpSocket(this);
	m_tg0 = new QTrafficGenerator(this);
	m_tg1 = new QTrafficGenerator(this);
	m_lm0 = new QLatencyMeasurer(this);
	m_sc0 = new QStatsCollector(this);
	m_sc1 = new QStatsCollector(this);
	m_sc2 = new QStatsCollector(this);
	m_sc3 = new QStatsCollector(this);

	connect(m_socket, &QTcpSocket::connected, this, &ZBNT::onConnected);
	connect(m_socket, &QTcpSocket::disconnected, this, &ZBNT::onDisconnected);
	connect(m_socket, &QTcpSocket::readyRead, this, &ZBNT::readSocket);
}

ZBNT::~ZBNT()
{ }

QString ZBNT::cyclesToTime(QString cycles)
{
	QString res;
	::cyclesToTime(cycles.toULongLong(), res);
	return res;
}

void ZBNT::autodetectBoardIP()
{
}

void ZBNT::connectToBoard()
{
}

void ZBNT::disconnectFromBoard()
{
}

void ZBNT::startRun()
{
}

void ZBNT::stopRun()
{
}

QString ZBNT::runTime()
{
	return QString::number(m_runTime);
}

void ZBNT::setRunTime(QString time)
{
	m_runTime = time.toULongLong();
}

QString ZBNT::currentTime()
{
	return QString::number(m_currentTime);
}

void ZBNT::setCurrentTime(QString time)
{
	m_currentTime = time.toULongLong();
}

void ZBNT::onConnected()
{
}

void ZBNT::onDisconnected()
{
}

void ZBNT::readSocket()
{
}
