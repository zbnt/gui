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

#include <QFrameDetector.hpp>

#include <QFile>

#include <Utils.hpp>
#include <Messages.hpp>

QFrameDetector::QFrameDetector(QObject *parent) : QObject(parent)
{
	for(int i = 0; i < 8; ++i)
	{
		m_patternPath.append("");
	}

	for(int i = 0; i < PATTERN_MEM_SIZE; ++i)
	{
		m_patternDataA[i] = 0;
		m_patternDataB[i] = 0;

		m_patternFlagsA[i] = 1;
		m_patternFlagsB[i] = 1;
	}
}

QFrameDetector::~QFrameDetector()
{ }

void QFrameDetector::enableLogging(const QString &fileName)
{
	disableLogging();

	m_logFile.setFileName(fileName);
	m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
}

void QFrameDetector::disableLogging()
{
	if(m_logFile.isOpen()) m_logFile.close();
}

void QFrameDetector::updateDisplayedValues()
{
	QMutexLocker lock(&m_mutex);

	m_displayedValues = m_currentValues;

	emit measurementChanged();
}

void QFrameDetector::appendSettings(QByteArray *buffer)
{
	if(!buffer) return;

	quint8 enable = 0;

	for(int i = 0; i < 8; ++i)
	{
		if(m_patternPath[i].toString().length())
		{
			enable |= 1 << i;
		}
	}

	buffer->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(buffer, MSG_ID_FD_CFG);
	appendAsBytes<quint16>(buffer, 2);

	appendAsBytes<quint8>(buffer, !!enable);
	appendAsBytes<quint8>(buffer, enable);
}

void QFrameDetector::appendPatterns(QByteArray *buffer)
{
	if(!buffer) return;

	buffer->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(buffer, MSG_ID_FD_PATTERNS);
	appendAsBytes<quint16>(buffer, PATTERN_MEM_SIZE * 4 + 1);

	appendAsBytes<quint8>(buffer, 0);
	buffer->append(QByteArray((const char*) m_patternDataA, PATTERN_MEM_SIZE));
	buffer->append(QByteArray((const char*) m_patternFlagsA, PATTERN_MEM_SIZE));
	buffer->append(QByteArray((const char*) m_patternDataB, PATTERN_MEM_SIZE));
	buffer->append(QByteArray((const char*) m_patternFlagsB, PATTERN_MEM_SIZE));
}

void QFrameDetector::receiveMeasurement(const QByteArray &measurement)
{
	m_mutex.lock();

	m_currentValues.time = readAsNumber<quint64>(measurement, 0);
	m_currentValues.match_dir = readAsNumber<quint8>(measurement, 8);
	m_currentValues.match_mask = readAsNumber<quint8>(measurement, 9);

	quint8 ext_num = readAsNumber<quint8>(measurement, 10);
	if(ext_num > 16) ext_num = 16;

	m_currentValues.match_ext_data = measurement.mid(12, ext_num);

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(m_currentValues.time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.match_dir));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.match_mask));
		m_logFile.putChar(',');
		m_logFile.write(m_currentValues.match_ext_data.toHex());
		m_logFile.putChar('\n');
	}
}

void QFrameDetector::loadPattern(quint32 id, QUrl url)
{
	if(id >= 8) return;

	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
		return;

	QFile patternFile;
	patternFile.setFileName(selectedPath);
	patternFile.open(QIODevice::ReadOnly);

	if(!patternFile.isOpen())
		return;

	m_patternPath[id] = selectedPath;

	quint8 *patternData = (id < 4 ? m_patternDataA : m_patternDataB);
	quint8 *patternFlags = (id < 4 ? m_patternFlagsA : m_patternFlagsB);
	if(id >= 4) id -= 4;

	QByteArray fileContents = patternFile.readAll();
	uint32_t num = 0x10, i = 0;
	bool inX = false;

	for(char c : fileContents)
	{
		if(i >= PATTERN_MEM_SIZE) break;

		patternData[i] = 0;
		patternFlags[i] = 0;

		if(inX)
		{
			inX = false;
			continue;
		}

		if(c == '$')
		{
			if(i < 4)
			{
				patternFlags[i] |= 0x2;
				i += 4;
			}
			else
			{
				patternFlags[i-4] |= 0x2;
			}

			break;
		}
		else if(c == '!')
		{
			if(i >= 4)
			{
				patternFlags[i-4] |= 0x5;
			}
		}
		else if(c == '?')
		{
			inX = true;
			patternFlags[i] |= 0x11;
			i += 4;
		}
		else if(c == 'r' || c == 'R')
		{
			inX = true;
			patternFlags[i] |= 0x9;
			i += 4;
		}
		else if(c == 'x' || c == 'X')
		{
			inX = true;
			patternFlags[i] |= 0x1;
			i += 4;
		}
		else if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
		{
			c = tolower(c) - '0';

			if(c >= 10)
				c -= 'a' - '9' - 1;

			if(num <= 0xF)
			{
				patternData[i] = (num << 4) | c;
				num = 0x10;
				i += 4;
			}
			else
			{
				num = c;
			}
		}
	}

	for(; i < PATTERN_MEM_SIZE; i += 4)
	{
		patternData[i] = 0;
		patternFlags[i] = 1;
	}

	emit patternsChanged();
}

void QFrameDetector::removePattern(quint32 id)
{
	if(id >= 8) return;

	m_patternPath[id] = "";

	if(id < 4)
	{
		for(int i = id; i < PATTERN_MEM_SIZE; i += 4)
		{
			m_patternDataA[i] = 0;
			m_patternFlagsA[i] = 1;
		}
	}
	else
	{
		id -= 4;

		for(int i = id; i < PATTERN_MEM_SIZE; i += 4)
		{
			m_patternDataB[i] = 0;
			m_patternFlagsB[i] = 1;
		}
	}

	emit patternsChanged();
}
