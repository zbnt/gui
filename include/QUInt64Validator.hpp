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

#include <QValidator>

#include <limits>

class QUInt64Validator : public QValidator
{
	Q_OBJECT

	Q_PROPERTY(QString top READ top WRITE setTop NOTIFY topChanged)
	Q_PROPERTY(QString bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
	Q_PROPERTY(QString error MEMBER m_error NOTIFY errorChanged)

public:
	QUInt64Validator(QObject *parent = nullptr);
	~QUInt64Validator();

	QString top();
	void setTop(QString top);

	QString bottom();
	void setBottom(QString bottom);

public slots:
	bool validate(QString input);
	QValidator::State validate(QString &input, int &pos) const override;

signals:
	void topChanged(QString top);
	void bottomChanged(QString bottom);
	void errorChanged(QString error);

private:
	qulonglong m_top = std::numeric_limits<qulonglong>::max();
	qulonglong m_bottom = 0;
	QString m_error;
};
