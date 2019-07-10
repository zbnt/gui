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

#pragma once

#include <QObject>
#include <QUrl>

class QTrafficGenerator : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool headersLoaded MEMBER m_headersLoaded NOTIFY headersChanged)
	Q_PROPERTY(quint32 headersLength MEMBER m_headersLength NOTIFY headersChanged)
	Q_PROPERTY(QString headersPath MEMBER m_headersPath NOTIFY headersChanged)

	Q_PROPERTY(quint8 enable MEMBER m_enable NOTIFY settingsChanged)

	Q_PROPERTY(quint8 paddingMethod MEMBER m_paddingMethod NOTIFY settingsChanged)
	Q_PROPERTY(quint16 paddingConstant MEMBER m_paddingConstant NOTIFY settingsChanged)
	Q_PROPERTY(quint16 paddingRangeTop MEMBER m_paddingRangeTop NOTIFY settingsChanged)
	Q_PROPERTY(quint16 paddingRangeBottom MEMBER m_paddingRangeBottom NOTIFY settingsChanged)
	Q_PROPERTY(quint16 paddingAverage MEMBER m_paddingAverage NOTIFY settingsChanged)

	Q_PROPERTY(quint8 delayMethod MEMBER m_delayMethod NOTIFY settingsChanged)
	Q_PROPERTY(QString delayConstant MEMBER m_delayConstant NOTIFY settingsChanged)
	Q_PROPERTY(QString delayRangeTop MEMBER m_delayRangeTop NOTIFY settingsChanged)
	Q_PROPERTY(QString delayRangeBottom MEMBER m_delayRangeBottom NOTIFY settingsChanged)
	Q_PROPERTY(QString delayAverage MEMBER m_delayAverage NOTIFY settingsChanged)

public:
	QTrafficGenerator(QObject *parent = nullptr);
	~QTrafficGenerator();

	void setIndex(quint8 idx);

	void appendSettings(QByteArray *buffer);
	void appendHeaders(QByteArray *buffer);

public slots:
	void loadHeaders(QUrl url);

signals:
	void headersChanged();
	void settingsChanged();

private:
	bool m_headersLoaded = false;
	quint32 m_headersLength = 0;
	QString m_headersPath;
	QByteArray m_headers;

	quint8 m_idx = 0;
	quint8 m_enable = 1;

	quint8 m_paddingMethod = 0;
	quint16 m_paddingConstant = 0;
	quint16 m_paddingRangeTop = 0;
	quint16 m_paddingRangeBottom = 0;
	quint16 m_paddingAverage = 0;

	quint8 m_delayMethod = 0;
	QString m_delayConstant;
	QString m_delayRangeTop;
	QString m_delayRangeBottom;
	QString m_delayAverage;
};
