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

QStatsCollector::QStatsCollector(QObject *parent) : QObject(parent)
{ }

QStatsCollector::~QStatsCollector()
{ }

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
