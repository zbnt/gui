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

#include <QUInt64Validator.hpp>

#include <algorithm>

QUInt64Validator::QUInt64Validator(QObject *parent) : QValidator(parent)
{ }

QUInt64Validator::~QUInt64Validator()
{ }

QString QUInt64Validator::top()
{
	return QString::number(m_top);
}

void QUInt64Validator::setTop(QString top)
{
	m_top = top.toULongLong();
	emit topChanged(top);
}

QString QUInt64Validator::bottom()
{
	return QString::number(m_bottom);
}

void QUInt64Validator::setBottom(QString bottom)
{
	m_bottom = bottom.toULongLong();
	emit bottomChanged(bottom);
}

bool QUInt64Validator::validate(QString input)
{
	int pos = 0;
	bool ok = false;
	qulonglong number = 0;

	validate(input, pos);
	number = input.toULongLong(&ok);

	if(!input.length() || number < m_bottom)
	{
		m_error = QString("Must be at least %1").arg(m_bottom);
		emit errorChanged(m_error);
		return false;
	}

	if(!ok || number > m_top)
	{
		m_error = QString("Must be at most %1").arg(m_top);
		emit errorChanged(m_error);
		return false;
	}

	m_error = "";
	emit errorChanged(m_error);
	return true;
}

QValidator::State QUInt64Validator::validate(QString &input, int &pos) const
{
	auto newEnd = std::remove_if(input.begin(), input.end(), [](const QChar &c) { return !c.isDigit(); } );
	input.truncate(newEnd - input.begin());
	return Acceptable;
}
