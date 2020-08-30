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

#include <QIPValidator.hpp>

#include <algorithm>

#include <QHostAddress>

QIPValidator::QIPValidator(QObject *parent) : QValidator(parent)
{ }

QIPValidator::~QIPValidator()
{ }

bool QIPValidator::validate(QString input)
{
	int pos = 0;
	validate(input, pos);

	QHostAddress addr(input);
	return !addr.isNull() && input.contains(QRegularExpression("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$"));
}

QValidator::State QIPValidator::validate(QString &input, int &pos) const
{
	auto newEnd = std::remove_if(input.begin(), input.end(), [](const QChar &c) { return !c.isDigit() && c != '.'; } );
	input.truncate(newEnd - input.begin());
	return Acceptable;
}
