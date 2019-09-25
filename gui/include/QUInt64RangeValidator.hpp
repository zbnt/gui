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

#include <QObject>

#include <limits>

class QUInt64RangeValidator : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString top READ top WRITE setTop NOTIFY topChanged)
	Q_PROPERTY(QString bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
	Q_PROPERTY(bool valid MEMBER m_valid NOTIFY validChanged)

public:
	QUInt64RangeValidator(QObject *parent = nullptr);
	~QUInt64RangeValidator();

	QString top();
	void setTop(QString top);

	QString bottom();
	void setBottom(QString bottom);

public slots:
	void validate();

signals:
	void topChanged(QString top);
	void bottomChanged(QString bottom);
	void validChanged(bool valid);

private:
	qulonglong m_top = std::numeric_limits<qulonglong>::max();
	qulonglong m_bottom = 0;
	bool m_valid = false;
};
