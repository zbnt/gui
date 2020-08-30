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

#include <QUInt64RangeValidator.hpp>

#include <algorithm>

QUInt64RangeValidator::QUInt64RangeValidator(QObject *parent) : QObject(parent)
{ }

QUInt64RangeValidator::~QUInt64RangeValidator()
{ }

QString QUInt64RangeValidator::top()
{
	return QString::number(m_top);
}

void QUInt64RangeValidator::setTop(QString top)
{
	m_top = top.toULongLong();
	emit topChanged(top);
	validate();
}

QString QUInt64RangeValidator::bottom()
{
	return QString::number(m_bottom);
}

void QUInt64RangeValidator::setBottom(QString bottom)
{
	m_bottom = bottom.toULongLong();
	emit bottomChanged(bottom);
	validate();
}

void QUInt64RangeValidator::validate()
{
	m_valid = (m_top >= m_bottom);
	emit validChanged(m_valid);
}
