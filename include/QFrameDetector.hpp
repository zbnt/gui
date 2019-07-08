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
#include <QFile>
#include <QList>
#include <QVariant>
#include <QTcpSocket>

#define PATTERN_MEM_SIZE 1536

class QFrameDetector : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVariantList patternPath MEMBER m_patternPath NOTIFY patternsChanged)

public:
	QFrameDetector(QObject *parent = nullptr);
	~QFrameDetector();

	void enableLogging(const QString &fileName);
	void disableLogging();

	void sendSettings(QTcpSocket *socket);
	void sendPatterns(QTcpSocket *socket);
	void receiveMeasurement(const QByteArray &measurement);

public slots:
	void loadPattern(quint32 id, QUrl url);
	void removePattern(quint32 id);

signals:
	void patternsChanged();

private:
	QVariantList m_patternPath;
	quint32 m_patternsA[PATTERN_MEM_SIZE];
	quint32 m_patternsB[PATTERN_MEM_SIZE];
	QFile m_logFile;
};
