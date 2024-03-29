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
#include <QList>
#include <QMutex>
#include <QVariant>
#include <QBitArray>
#include <QIODevice>
#include <QTextStream>

#include <QTableModel.hpp>
#include <dev/QAbstractDevice.hpp>

class QFrameDetector : public QAbstractDevice
{
	Q_OBJECT

	Q_PROPERTY(quint8 portA MEMBER m_portA NOTIFY settingsChanged)
	Q_PROPERTY(quint8 portB MEMBER m_portB NOTIFY settingsChanged)
	Q_PROPERTY(quint32 featureBits MEMBER m_featureBits NOTIFY settingsChanged)
	Q_PROPERTY(quint32 numScripts MEMBER m_numScripts NOTIFY settingsChanged)
	Q_PROPERTY(quint32 maxScriptSize MEMBER m_maxScriptSize NOTIFY settingsChanged)
	Q_PROPERTY(quint32 txFifoSize MEMBER m_txFifoSize NOTIFY settingsChanged)
	Q_PROPERTY(quint32 extrFifoSize MEMBER m_extrFifoSize NOTIFY settingsChanged)

	Q_PROPERTY(quint8 enable MEMBER m_enable NOTIFY settingsChanged)
	Q_PROPERTY(quint8 logEnable MEMBER m_logEnable NOTIFY settingsChanged)
	Q_PROPERTY(QStringList scriptName MEMBER m_scriptName NOTIFY scriptsChanged)
	Q_PROPERTY(QVariantList scriptPath MEMBER m_scriptPath NOTIFY scriptsChanged)
	Q_PROPERTY(quint32 scriptsEnabled MEMBER m_scriptsEnabled NOTIFY scriptsChanged)
	Q_PROPERTY(bool fixChecksums MEMBER m_fixChecksums NOTIFY settingsChanged)

	Q_PROPERTY(QStringList detectionCounters MEMBER m_detectionCountersStr NOTIFY measurementChanged)
	Q_PROPERTY(QTableModel *detectionListA MEMBER m_detectionListA NOTIFY measurementChanged)
	Q_PROPERTY(QTableModel *detectionListB MEMBER m_detectionListB NOTIFY measurementChanged)

	struct Script
	{
		QVector<quint16> comparator;
		QVector<quint16> editor;
	};

public:
	QFrameDetector(QObject *parent = nullptr);
	~QFrameDetector();

	void loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props);

	quint32 setPcapOutput(std::shared_ptr<QIODevice> &output, quint32 index);
	void enableLogging(const QString &path);
	void disableLogging();

	void updateDisplayedValues();

	void receiveMeasurement(const QByteArray &measurement);
	void resetMeasurement();

public slots:
	QString description() const;
	QString settingsQml() const;
	QString statusQml() const;

	QByteArray loadScript(quint32 id, QUrl url);
	void removeScript(quint32 id);

private:
	bool parseScript(QTextStream &input, Script &script, QString &error) const;

	bool parseScriptLine(const QVector<QStringRef> &pieces, Script &script, quint32 &offset, int &inSection, QString &error) const;
	bool parseComparatorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const;
	bool parseExtractorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const;
	bool parseEditorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const;

signals:
	void scriptsChanged();
	void settingsChanged();
	void measurementChanged();
	void error(const QString &msg);

private:
	quint8 m_portA = 0;
	quint8 m_portB = 0;
	quint32 m_featureBits = 0;
	quint32 m_numScripts = 0;
	quint32 m_maxScriptSize = 0;
	quint32 m_txFifoSize = 0;
	quint32 m_extrFifoSize = 0;

	quint8 m_enable = 0;
	quint8 m_logEnable = 0;
	QStringList m_scriptName;
	QVariantList m_scriptPath;
	quint32 m_scriptsEnabled;
	bool m_fixChecksums = true;

	QList<QStringList> m_pendingDetections[2];
	QList<quint64> m_detectionCounters;

	QTableModel *m_detectionListA = nullptr;
	QTableModel *m_detectionListB = nullptr;
	QStringList m_detectionCountersStr;

	QMutex m_mutex;
	quint32 m_logIndex;
	std::shared_ptr<QIODevice> m_logOutput;
};
