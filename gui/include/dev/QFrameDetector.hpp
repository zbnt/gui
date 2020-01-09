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

#include <QTableModel.hpp>
#include <dev/QAbstractDevice.hpp>

class QFrameDetector : public QAbstractDevice
{
	Q_OBJECT

	Q_PROPERTY(QVariantList patternPath MEMBER m_patternPath NOTIFY patternsChanged)
	Q_PROPERTY(QVariantList patternBytes MEMBER m_patternBytes NOTIFY patternsChanged)
	Q_PROPERTY(QVariantList patternFlags MEMBER m_patternFlags NOTIFY patternsChanged)
	Q_PROPERTY(quint16 numPatterns MEMBER m_numPatterns NOTIFY patternsChanged)
	Q_PROPERTY(quint16 maxPatternLength MEMBER m_maxPatternLength NOTIFY patternsChanged)
	Q_PROPERTY(bool fixChecksums MEMBER m_fixChecksums NOTIFY settingsChanged)

	Q_PROPERTY(QStringList detectionCounters MEMBER m_detectionCountersStr NOTIFY measurementChanged)
	Q_PROPERTY(QTableModel *detectionListA MEMBER m_detectionListA NOTIFY measurementChanged)
	Q_PROPERTY(QTableModel *detectionListB MEMBER m_detectionListB NOTIFY measurementChanged)

public:
	QFrameDetector(QObject *parent = nullptr);
	~QFrameDetector();

	void setExtraInfo(quint64 values);

	void enableLogging(const QString &path);
	void disableLogging();

	void updateDisplayedValues();

	void receiveMeasurement(const QByteArray &measurement);
	void resetMeasurement();

public slots:
	QString description() const;
	QString settingsQml() const;
	QString statusQml() const;

	bool loadPattern(quint32 id, QUrl url);
	void removePattern(quint32 id);

signals:
	void patternsChanged();
	void settingsChanged();
	void measurementChanged();
	void error(const QString &msg);

private:
	QVariantList m_patternPath;
	QVariantList m_patternBytes;
	QVariantList m_patternFlags;
	quint16 m_numPatterns;
	quint16 m_maxPatternLength;
	bool m_fixChecksums = true;

	QList<QStringList> m_pendingDetections[2];
	QList<quint64> m_detectionCounters;

	QTableModel *m_detectionListA = nullptr;
	QTableModel *m_detectionListB = nullptr;
	QStringList m_detectionCountersStr;

	QMutex m_mutex;
	QFile m_logFile;
};
