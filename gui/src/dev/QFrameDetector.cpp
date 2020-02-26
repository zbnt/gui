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

#include <dev/QFrameDetector.hpp>

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#include <Utils.hpp>
#include <Messages.hpp>

QFrameDetector::QFrameDetector(QObject *parent)
	: QAbstractDevice(parent)
{
	for(int i = 0; i < 8; ++i)
	{
		m_scriptName.append("");
		m_detectionCounters.append(0);
		m_detectionCountersStr.append("0");
	}

	m_scriptsEnabled = 0;
	m_detectionListA = new QTableModel(3, this);
	m_detectionListB = new QTableModel(3, this);
}

QFrameDetector::~QFrameDetector()
{ }

void QFrameDetector::loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props)
{
	for(auto prop : props)
	{
		switch(prop.first)
		{
			case PROP_PORTS:
			{
				if(prop.second.size() < 2) break;

				m_portA = readAsNumber<quint8>(prop.second, 0);
				m_portB = readAsNumber<quint8>(prop.second, 1);
				break;
			}

			case PROP_NUM_SCRIPTS:
			{
				if(prop.second.size() < 4) break;

				m_numScripts = readAsNumber<quint32>(prop.second, 0);
				break;
			}

			case PROP_MAX_SCRIPT_SIZE:
			{
				if(prop.second.size() < 4) break;

				m_maxScriptSize = readAsNumber<quint32>(prop.second, 0);
				break;
			}

			default: { }
		}
	}

	for(quint32 i = 0; i < m_numScripts*2; ++i)
	{
		m_scriptName.append("");
		m_scriptBytes.append(QByteArray());
		m_patternFlags.append(QByteArray());
	}

	emit settingsChanged();
}

void QFrameDetector::enableLogging(const QString &path)
{
	disableLogging();

	m_logFile.setFileName(path + QString("/eth%1_eth%2_detector.csv").arg(m_portA).arg(m_portB));
	m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

void QFrameDetector::disableLogging()
{
	if(m_logFile.isOpen()) m_logFile.close();
}

void QFrameDetector::updateDisplayedValues()
{
	m_mutex.lock();

	m_detectionListA->prependRows(m_pendingDetections[0]);
	m_detectionListB->prependRows(m_pendingDetections[1]);
	m_pendingDetections[0].clear();
	m_pendingDetections[1].clear();

	for(int i = 0; i < 8; ++i)
	{
		m_detectionCountersStr[i] = QString::number(m_detectionCounters[i]);
	}

	if(m_detectionListA->rowCount() > 1000)
	{
		m_detectionListA->removeRows(1000, m_detectionListA->rowCount());
	}

	if(m_detectionListB->rowCount() > 1000)
	{
		m_detectionListB->removeRows(1000, m_detectionListB->rowCount());
	}

	m_mutex.unlock();

	emit measurementChanged();
}

void QFrameDetector::receiveMeasurement(const QByteArray &measurement)
{
	if(measurement.size() < 12) return;

	quint64 time = readAsNumber<quint64>(measurement, 0);
	quint8 match_dir = readAsNumber<quint8>(measurement, 8);
	quint8 match_mask = readAsNumber<quint8>(measurement, 9);
	quint16 ext_num = readAsNumber<quint16>(measurement, 10);
	QByteArray match_ext_data = measurement.mid(12, ext_num).toHex();

	if(ext_num > 16)
	{
		ext_num = 16;
	}

	match_dir -= 'A';

	if(match_dir > 1)
	{
		return;
	}

	QString match_mask_str;
	bool first = true;

	for(int i = 0; i < 4; ++i)
	{
		if(match_mask & (1 << i))
		{
			if(!first) match_mask_str.append(", ");
			match_mask_str.append(QString::number(i + 1));
		}

		first = false;
	}

	QStringList matchInfo =
	{
		QString::number(time * 8),
		match_mask_str,
		QString::fromUtf8(match_ext_data),
	};

	m_mutex.lock();

	m_pendingDetections[match_dir].append(matchInfo);

	for(int i = 0; i < 4; ++i)
	{
		if(match_mask & (1 << i))
		{
			m_detectionCounters[i + 4*match_dir] += 1;
		}
	}

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(match_dir));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(match_mask));
		m_logFile.putChar(',');
		m_logFile.write(match_ext_data);
		m_logFile.putChar('\n');
	}
}

void QFrameDetector::resetMeasurement()
{
	QMutexLocker lock(&m_mutex);

	m_pendingDetections[0].clear();
	m_pendingDetections[1].clear();
	m_detectionListA->clearRows();
	m_detectionListB->clearRows();

	for(int i = 0; i < m_detectionCounters.size(); ++i)
	{
		m_detectionCounters[i] = 0;
	}
}

QString QFrameDetector::description() const
{
	return QString("Frame detector (eth%1, eth%2)").arg(m_portA).arg(m_portB);
}

QString QFrameDetector::settingsQml() const
{
	return QString("qrc:/qml/SettingsTabFD.qml");
}

QString QFrameDetector::statusQml() const
{
	return QString("qrc:/qml/StatusTabFD.qml");
}

bool QFrameDetector::loadScript(quint32 id, QUrl url)
{
	if(id >= m_numScripts*2)
	{
		emit error("Invalid script ID");
		return false;
	}

	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
	{
		emit error("No file selected");
		return false;
	}

	QFile scriptFile;
	scriptFile.setFileName(selectedPath);
	scriptFile.open(QIODevice::ReadOnly | QIODevice::Text);

	if(!scriptFile.isOpen())
	{
		emit error("Can't read script file, make sure you have the required permissions");
		return false;
	}

	Script script;
	QString errorMessage;
	QTextStream fileInput(&scriptFile);

	script.comparator.fill(0, m_maxScriptSize);
	script.editor.fill(0, m_maxScriptSize);

	if(!parseScript(fileInput, script, errorMessage))
	{
		emit error(errorMessage);
		return false;
	}

	QByteArray scriptBytes;

	for(quint32 i = 0; i < m_maxScriptSize; ++i)
	{
		quint32 fullInstr;

		fullInstr = script.comparator[i];
		fullInstr |= quint32(script.editor[i]) << 16;

		appendAsBytes(scriptBytes, fullInstr);
	}

	m_scriptName[id] = url.fileName();
	m_scriptBytes[id] = scriptBytes;
	m_scriptsEnabled |= (1 << id);
	emit scriptsChanged();

	return true;
}

void QFrameDetector::removeScript(quint32 id)
{
	if(id >= m_numScripts*2) return;

	QByteArray patternData;
	patternData.append(id >= m_numScripts);
	patternData.append(id % m_numScripts);

	m_scriptName[id] = "";
	m_scriptBytes[id] = patternData;
	m_patternFlags[id] = patternData;
	m_scriptsEnabled &= ~(1 << id);
	emit scriptsChanged();
}

bool QFrameDetector::parseScript(QTextStream &input, Script &script, QString &error) const
{
	QString line;
	quint32 offset = 0;
	int commentStart = -1, lineCount = 1, inSection = 0;

	while(!input.atEnd())
	{
		// Read and prepare line

		input.readLineInto(&line);

		if((commentStart = line.indexOf('#')) != -1)
		{
			line.remove(commentStart, line.size() - commentStart);
		}

		line = line.simplified();

		// Parse instruction

		QVector<QStringRef> pieces = line.splitRef(' ', QString::SkipEmptyParts);

		switch(pieces.size())
		{
			case 1:
			case 2:
			{
				if(!parseScriptLine(pieces, script, offset, inSection, error))
				{
					error = QString("line %1: ").arg(lineCount) + error;
					return false;
				}

				break;
			}

			case 0:
			{
				break;
			}

			default:
			{
				error = QString("line %1: Invalid syntax").arg(lineCount);
				return false;
			}
		}

		++lineCount;
	}

	return true;
}

bool QFrameDetector::parseScriptLine(const QVector<QStringRef> &pieces, Script &script, quint32 &offset, int &inSection, QString &error) const
{
	QStringRef param = (pieces.size() == 2) ? pieces[1] : QStringRef();
	bool ok = false;

	if(pieces[0].startsWith("."))
	{
		if(pieces[0] == ".comp")
		{
			inSection = 1;
		}
		else if(pieces[0] == ".extr")
		{
			inSection = 2;
		}
		else if(pieces[0] == ".edit")
		{
			inSection = 3;
		}
		else
		{
			error = "Invalid section";
			return false;
		}

		if(pieces.size() == 2)
		{
			offset = pieces[1].toUInt(&ok, 0);

			if(!ok)
			{
				error = "Invalid offset";
				return false;
			}

			if(offset >= m_maxScriptSize)
			{
				error = "Offset is out of range";
				return false;
			}
		}
		else
		{
			offset = 0;
		}

		return true;
	}

	switch(inSection)
	{
		case 0:
		{
			error = "Invalid instruction, not inside a section";
			return false;
		}

		case 1:
		{
			if(!parseComparatorInstr(pieces[0], param, script, offset, error))
			{
				return false;
			}

			break;
		}

		case 2:
		{
			if(!parseExtractorInstr(pieces[0], param, script, offset, error))
			{
				return false;
			}

			break;
		}

		case 3:
		{
			if(!parseEditorInstr(pieces[0], param, script, offset, error))
			{
				return false;
			}

			break;
		}
	}

	return true;
}

bool QFrameDetector::parseComparatorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const
{
	static const QRegularExpression instrRegex(R"(^(?:nop|(s?[lg]tq?|eq|or|and)(8|16|24|32|40|48|56|64|[fd])(l?)|eof)$)");
	static const QVector<QString> opList = {"eq", "gt", "lt", "gtq", "ltq", "or", "and"};

	QRegularExpressionMatch regexMatch = instrRegex.match(instr);
	bool ok = false;

	if(!regexMatch.hasMatch())
	{
		error = "Unknown instruction: " + instr;
		return false;
	}

	if(instr == "nop")
	{
		if(param.size())
		{
			quint32 paramInt = param.toUInt(&ok);

			if(!ok)
			{
				error = "Invalid parameter, must be an unsigned integer";
				return false;
			}

			if(offset + paramInt >= m_maxScriptSize)
			{
				error = "Instruction ends beyond the script size limit";
				return false;
			}

			do
			{
				script.comparator[offset++] = 0;
			}
			while(--paramInt);
		}
		else
		{
			++offset;
		}
	}
	else if(instr == "eof")
	{
		script.comparator[offset++] = 0b11110010;
	}
	else
	{
		if(!param.size())
		{
			error = "Instruction requires a parameter";
			return false;
		}

		QString base = regexMatch.captured(1);
		QString size = regexMatch.captured(2);
		QByteArray value;
		quint16 opcode = 0;
		int sizeInt = 0;

		if(!regexMatch.capturedRef(3).size())
		{
			// Big-endian
			opcode |= 0b100;
		}

		if(base.startsWith("s"))
		{
			// Signed
			opcode |= 0b1000;
			base.remove(0, 1);
		}

		if(size == "f")
		{
			sizeInt = 4;
			appendAsBytes(value, param.toFloat(&ok));
		}
		else if(size == "d")
		{
			sizeInt = 8;
			appendAsBytes(value, param.toDouble(&ok));
		}
		else
		{
			sizeInt = size.toInt() / 8;

			if(opcode & 0b1000)
			{
				appendAsBytes(value, param.toLongLong(&ok, 0));

				for(int i = sizeInt; i < 8; ++i)
				{
					if(value.at(i) != value.at(sizeInt) || value.at(i) != -1 || value.at(i) != 0)
					{
						ok = false;
						break;
					}
				}
			}
			else
			{
				appendAsBytes(value, param.toULongLong(&ok, 0));

				for(int i = sizeInt; i < 8; ++i)
				{
					if(value.at(i) != 0)
					{
						ok = false;
						break;
					}
				}
			}
		}

		if(!ok)
		{
			error = "Invalid parameter";
			return false;
		}

		opcode |= opList.indexOf(base) << 4;

		for(int i = 0; i < sizeInt; ++i)
		{
			if(offset >= m_maxScriptSize)
			{
				error = "Instruction ends beyond the script size limit";
				return false;
			}

			if(opcode & 0b100)
			{
				script.comparator[offset] = opcode | (quint16(value[sizeInt-i-1]) << 8);
			}
			else
			{
				script.comparator[offset] = opcode | (quint16(value[i]) << 8);
			}

			if(i == sizeInt - 1)
			{
				script.comparator[offset] |= 0b11;
			}
			else
			{
				script.comparator[offset] |= 0b01;
			}

			++offset;
		}
	}

	return true;
}

bool QFrameDetector::parseExtractorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const
{
	static const QRegularExpression instrRegex(R"(^(?:nop|ext)$)");
	QRegularExpressionMatch regexMatch = instrRegex.match(instr);
	bool ok = false;

	if(!regexMatch.hasMatch())
	{
		error = "Unknown instruction: " + instr;
		return false;
	}

	if(param.size())
	{
		quint32 paramInt = param.toUInt(&ok);

		if(!ok)
		{
			error = "Invalid parameter, must be an unsigned integer";
			return false;
		}

		if(offset + paramInt >= m_maxScriptSize)
		{
			error = "Instruction ends beyond the script size limit";
			return false;
		}

		if(instr == "ext")
		{
			do
			{
				script.editor[offset++] |= 1;
			}
			while(--paramInt);
		}
		else
		{
			do
			{
				script.editor[offset++] &= ~1;
			}
			while(--paramInt);
		}
	}

	return true;
}

bool QFrameDetector::parseEditorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const
{
	const QRegularExpression instrRegex(R"(^(?:nop|(setr?|(?:xn)?or|and|add|s?mul)(8|16|32|64|[fd])(l?)|drop|corrupt)$)");
	QRegularExpressionMatch regexMatch = instrRegex.match(instr);
	bool ok = false;

	static const QHash<QString, quint16> opMap =
	{
		{"nop", 0b00'00'000'0},
		{"set", 0b00'00'001'0},
		{"setr", 0b00'01'001'0},
		{"and", 0b00'00'010'0},
		{"or", 0b00'01'010'0},
		{"xor", 0b00'10'010'0},
		{"xnor", 0b00'11'010'0},
		{"add", 0b00'00'011'0},
		{"mul", 0b00'00'100'0},
		{"smul", 0b00'10'100'0},
		{"drop", 0b00'10'000'0},
		{"corrupt", 0b00'01'000'0}
	};

	if(!regexMatch.hasMatch())
	{
		error = "Unknown instruction: " + instr;
		return false;
	}

	if(instr == "nop")
	{
		if(param.size())
		{
			quint32 paramInt = param.toUInt(&ok);

			if(!ok)
			{
				error = "Invalid parameter, must be an unsigned integer";
				return false;
			}

			if(offset + paramInt >= m_maxScriptSize)
			{
				error = "Instruction ends beyond the script size limit";
				return false;
			}

			do
			{
				script.editor[offset++] = 0;
			}
			while(--paramInt);
		}
		else
		{
			++offset;
		}
	}
	else if(instr == "drop" || instr == "corrupt")
	{
		script.comparator[offset++] = opMap[instr.toString()];
	}
	else
	{
		if(!param.size())
		{
			error = "Instruction requires a parameter";
			return false;
		}

		QString base = regexMatch.captured(1);
		QString size = regexMatch.captured(2);
		QByteArray value;
		quint16 opcode = 0;
		int sizeInt = 0;

		opcode = opMap[instr.toString()];

		if(!regexMatch.capturedRef(3).size())
		{
			// Big-endian
			opcode |= 0b10000;
		}

		if(size == "f")
		{
			sizeInt = 4;
			appendAsBytes(value, param.toFloat(&ok));
		}
		else if(size == "d")
		{
			sizeInt = 8;
			appendAsBytes(value, param.toDouble(&ok));
		}
		else
		{
			sizeInt = size.toInt() / 8;

			if(opcode & 0b100000)
			{
				appendAsBytes(value, param.toLongLong(&ok, 0));

				for(int i = sizeInt; i < 8; ++i)
				{
					if(value.at(i) != value.at(sizeInt) || value.at(i) != -1 || value.at(i) != 0)
					{
						ok = false;
						break;
					}
				}
			}
			else
			{
				appendAsBytes(value, param.toULongLong(&ok, 0));

				for(int i = sizeInt; i < 8; ++i)
				{
					if(value.at(i) != 0)
					{
						ok = false;
						break;
					}
				}
			}
		}

		if(!ok)
		{
			error = "Invalid parameter";
			return false;
		}

		for(int i = 0; i < sizeInt; ++i)
		{
			if(offset >= m_maxScriptSize)
			{
				error = "Instruction ends beyond the script size limit";
				return false;
			}

			if(opcode & 0b10000)
			{
				script.editor[offset] = quint16(value[sizeInt-i-1]) << 8;
			}
			else
			{
				script.editor[offset] = quint16(value[i]) << 8;
			}

			if(i == sizeInt - 1)
			{
				script.editor[offset] |= opcode;
			}

			++offset;
		}
	}

	return true;
}
