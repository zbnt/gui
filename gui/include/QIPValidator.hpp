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

#include <QValidator>

#include <limits>

class QIPValidator : public QValidator
{
	Q_OBJECT

public:
	QIPValidator(QObject *parent = nullptr);
	~QIPValidator();

public slots:
	bool validate(QString input);
	QValidator::State validate(QString &input, int &pos) const override;
};