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

#include <dev/QFrameDetector.hpp>

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

#include <Messages.hpp>
#include <MessageUtils.hpp>

QFrameDetector::QFrameDetector(QObject *parent)
	: QAbstractDevice(parent)
{
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

			case PROP_FEATURE_BITS:
			{
				if(prop.second.size() < 4) break;

				m_featureBits = readAsNumber<quint32>(prop.second, 0);
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

			case PROP_FIFO_SIZE:
			{
				if(prop.second.size() < 8) break;

				m_txFifoSize = readAsNumber<quint32>(prop.second, 0);
				m_extrFifoSize = readAsNumber<quint32>(prop.second, 4);
				break;
			}

			default: { }
		}
	}

	for(quint32 i = 0; i < m_numScripts*2; ++i)
	{
		m_scriptName.append("");
		m_scriptPath.append("");
		m_detectionCounters.append(0);
		m_detectionCountersStr.append("0");
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

	for(quint32 i = 0; i < 2*m_numScripts; ++i)
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
	if(measurement.size() < 14) return;

	quint64 time = readAsNumber<quint64>(measurement, 0);
	quint8 matchDir = readAsNumber<quint8>(measurement, 8);
	quint8 logWidth = readAsNumber<quint8>(measurement, 9);
	quint16 extCount = readAsNumber<quint16>(measurement, 10);
	quint16 matchMask = readAsNumber<quint16>(measurement, 12);

	qint32 extOffset = ((logWidth + 13) / logWidth) * logWidth;

	matchDir -= 'A';

	if(matchDir > 1) return;
	if(measurement.size() % logWidth != 0) return;
	if(extCount > measurement.size() - extOffset) return;

	QString matchMaskStr;
	for(quint32 i = 0; i < m_numScripts; ++i)
	{
		matchMaskStr.prepend(QString::number(!!(matchMask & (1 << i))));
	}

	QStringList matchInfo =
	{
		QString::number(time * 8),
		matchMaskStr,
		QString("%1 bytes").arg(extCount),
	};

	QByteArray extractedData = measurement.mid(extOffset);

	if(extractedData.size() && extCount % logWidth != 0)
	{
		extractedData.remove(extractedData.size() - logWidth, logWidth - extCount % logWidth);
	}

	m_mutex.lock();

	m_pendingDetections[matchDir].append(matchInfo);

	for(quint32 i = 0; i < m_numScripts; ++i)
	{
		if(matchMask & (1 << i))
		{
			m_detectionCounters[i + m_numScripts*matchDir] += 1;
		}
	}

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(matchDir));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(matchMask));
		m_logFile.putChar(',');
		m_logFile.write(extractedData.toHex());
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

QByteArray QFrameDetector::loadScript(quint32 id, QUrl url)
{
	QByteArray scriptBytes;

	if(id >= m_numScripts*2)
	{
		emit error("Invalid script ID");
		return scriptBytes;
	}

	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
	{
		emit error("No file selected");
		return scriptBytes;
	}

	if(!selectedPath.endsWith(".zbscr"))
	{
		emit error("Invalid file selected");
		return scriptBytes;
	}

	QFile scriptFile;
	scriptFile.setFileName(selectedPath);
	scriptFile.open(QIODevice::ReadOnly | QIODevice::Text);

	if(!scriptFile.isOpen())
	{
		emit error("Can't read script file, make sure you have the required permissions");
		return scriptBytes;
	}

	Script script;
	QString errorMessage;
	QTextStream fileInput(&scriptFile);

	script.comparator.fill(0, m_maxScriptSize);
	script.editor.fill(0, m_maxScriptSize);

	if(!parseScript(fileInput, script, errorMessage))
	{
		emit error(errorMessage);
		return scriptBytes;
	}

	appendAsBytes(scriptBytes, id);

	for(quint32 i = 0; i < m_maxScriptSize; ++i)
	{
		quint32 fullInstr;

		fullInstr = script.comparator[i] & 0xFF;
		fullInstr |= (script.editor[i] & 0xFF) << 8;
		fullInstr |= quint32(script.comparator[i] & 0xFF00) << 8;
		fullInstr |= quint32(script.editor[i] & 0xFF00) << 16;

		appendAsBytes(scriptBytes, fullInstr);
	}

	m_scriptName[id] = url.fileName().chopped(6);
	m_scriptPath[id] = url;
	m_scriptsEnabled |= (1 << id);

	emit scriptsChanged();
	return scriptBytes;
}

void QFrameDetector::removeScript(quint32 id)
{
	if(id >= m_numScripts*2) return;

	m_scriptName[id] = "";
	m_scriptPath[id] = "";
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

		line = line.simplified().toLower();

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
		error = "Unknown comparator instruction: " + instr;
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

			if(offset + paramInt > m_maxScriptSize)
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
		error = "Unknown extractor instruction: " + instr;
		return false;
	}

	quint32 paramInt = 1;

	if(param.size())
	{
		paramInt = param.toUInt(&ok);

		if(!ok)
		{
			error = "Invalid parameter, must be an unsigned integer";
			return false;
		}

		if(offset + paramInt > m_maxScriptSize)
		{
			error = "Instruction ends beyond the script size limit";
			return false;
		}
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

	return true;
}

bool QFrameDetector::parseEditorInstr(const QStringRef &instr, const QStringRef &param, Script &script, quint32 &offset, QString &error) const
{
	const QRegularExpression instrRegex(R"(^(?:nop|setr|(set|(?:xn|x)?or|and|add|s?mul)(8|16|32|64|[fd])(l?)|drop|corrupt)$)");
	QRegularExpressionMatch regexMatch = instrRegex.match(instr);
	bool ok = false;

	static const QHash<QString, quint16> opMap =
	{
		{"nop",     0b00'00'000'0},
		{"set",     0b00'00'001'0},
		{"setr",    0b00'01'001'0},
		{"and",     0b00'00'010'0},
		{"or",      0b00'01'010'0},
		{"xor",     0b00'10'010'0},
		{"xnor",    0b00'11'010'0},
		{"add",     0b00'00'011'0},
		{"mul",     0b00'00'100'0},
		{"smul",    0b00'10'100'0},
		{"drop",    0b00'10'000'0},
		{"corrupt", 0b00'01'000'0}
	};

	if(!regexMatch.hasMatch())
	{
		error = "Unknown editor instruction: " + instr;
		return false;
	}

	if(instr == "nop" || instr == "setr")
	{
		if(param.size())
		{
			quint32 paramInt = param.toUInt(&ok);

			if(!ok)
			{
				error = "Invalid parameter, must be an unsigned integer";
				return false;
			}

			if(offset + paramInt > m_maxScriptSize)
			{
				error = "Instruction ends beyond the script size limit";
				return false;
			}

			do
			{
				script.editor[offset] &= 1;
				script.editor[offset] |= opMap[instr.toString()];
				offset++;
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
		script.editor[offset] = opMap[instr.toString()] | (script.editor[offset] & 1);
		offset++;
	}
	else
	{
		QString base = regexMatch.captured(1);
		QString size = regexMatch.captured(2);
		QByteArray value;
		quint16 opcode = 0;
		int sizeInt = 0;
		bool isBigEndian = false;
		bool isSigned = false;

		if(!param.size())
		{
			error = "Instruction requires a parameter";
			return false;
		}

		opcode = opMap[base];

		if(!regexMatch.capturedRef(3).size())
		{
			isBigEndian = true;

			if(base == "add" || base == "mul" || base == "smul")
			{
				// Big-endian
				opcode |= 0b10000;
			}
		}

		if((opcode & 0b0100) != 0b0100)
		{
			isSigned = opcode & 0b100000;
		}

		if(base == "add" && param[0] == "-")
		{
			isSigned = true;
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

			if(isSigned)
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

		switch(sizeInt)
		{
			case 2:
			{
				opcode |= 1 << 6;
				break;
			}

			case 4:
			{
				opcode |= 2 << 6;
				break;
			}

			case 8:
			{
				opcode |= 3 << 6;
				break;
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

			script.editor[offset] &= 1;

			if(isBigEndian)
			{
				script.editor[offset] |= quint16(value[sizeInt-i-1]) << 8;
			}
			else
			{
				script.editor[offset] |= quint16(value[i]) << 8;
			}

			if(i == 0)
			{
				script.editor[offset] |= opcode;
			}

			++offset;
		}
	}

	return true;
}
