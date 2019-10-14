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
#include <QMutex>
#include <QVariant>

#define PATTERN_MEM_SIZE 8192

class QFrameDetector : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVariantList patternPath MEMBER m_patternPath NOTIFY patternsChanged)

	struct Measurement
	{
		quint64 time = 0;
		quint8 match_dir;
		quint8 match_mask;
		QByteArray match_ext_data;
	};

public:
	QFrameDetector(QObject *parent = nullptr);
	~QFrameDetector();

	void enableLogging(const QString &fileName);
	void disableLogging();

	void updateDisplayedValues();

	void appendSettings(QByteArray *buffer);
	void appendPatterns(QByteArray *buffer);
	void receiveMeasurement(const QByteArray &measurement);

public slots:
	void loadPattern(quint32 id, QUrl url);
	void removePattern(quint32 id);

signals:
	void patternsChanged();
	void measurementChanged();

private:
	QVariantList m_patternPath;
	quint8 m_patternDataA[PATTERN_MEM_SIZE];
	quint8 m_patternDataB[PATTERN_MEM_SIZE];
	quint8 m_patternFlagsA[PATTERN_MEM_SIZE];
	quint8 m_patternFlagsB[PATTERN_MEM_SIZE];

	Measurement m_currentValues, m_displayedValues;
	QMutex m_mutex;

	QFile m_logFile;
};
