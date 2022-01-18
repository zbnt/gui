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

#pragma once

#include <QObject>
#include <QUrl>

#include <dev/QAbstractDevice.hpp>

class QTrafficGenerator : public QAbstractDevice
{
	Q_OBJECT

	Q_PROPERTY(bool templateLoaded MEMBER m_templateLoaded NOTIFY templateChanged)
	Q_PROPERTY(quint32 templateLength MEMBER m_templateLength NOTIFY templateChanged)
	Q_PROPERTY(QString templatePath MEMBER m_templatePath NOTIFY templateChanged)
	Q_PROPERTY(QByteArray templateBytes MEMBER m_templateBytes NOTIFY templateChanged)
	Q_PROPERTY(QByteArray sourceBytes MEMBER m_sourceBytes NOTIFY templateChanged)

	Q_PROPERTY(quint8 enable MEMBER m_enable NOTIFY settingsChanged)

	Q_PROPERTY(QString frameSize MEMBER m_frameSize NOTIFY settingsChanged)
	Q_PROPERTY(QString frameDelay MEMBER m_frameDelay NOTIFY settingsChanged)

	Q_PROPERTY(quint8 burstEnable MEMBER m_burstEnable NOTIFY settingsChanged)
	Q_PROPERTY(QString burstOnTime MEMBER m_burstOnTime NOTIFY settingsChanged)
	Q_PROPERTY(QString burstOffTime MEMBER m_burstOffTime NOTIFY settingsChanged)

	Q_PROPERTY(QString lfsrSeed MEMBER m_lfsrSeed NOTIFY settingsChanged)

public:
	QTrafficGenerator(QObject *parent = nullptr);
	~QTrafficGenerator();

	void loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props);

	void enableLogging(const QString &path);
	void disableLogging();

	void updateDisplayedValues();

	void receiveMeasurement(const QByteArray &measurement);
	void resetMeasurement();

public slots:
	QString description() const;
	QString settingsQml() const;
	QString statusQml() const;

	bool loadTemplate(QUrl url);
	void clearTemplate();

signals:
	void templateChanged();
	void settingsChanged();
	void error(const QString &msg);

private:
	bool m_templateLoaded = false;
	quint32 m_templateLength = 0;
	QString m_templatePath;
	QByteArray m_templateBytes;
	QByteArray m_sourceBytes;
	qint32 m_maxTemplateLength;

	quint8 m_port = 0;
	quint8 m_enable = 1;
	QString m_frameSize;
	QString m_frameDelay;

	quint8 m_burstEnable = 0;
	QString m_burstOnTime;
	QString m_burstOffTime;

	QString m_lfsrSeed;
};
