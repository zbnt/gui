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
	for(int i = 0; i < 6; ++i)
	{
		m_patternPath.append("");
	}

	for(int i = 0; i < PATTERN_MEM_SIZE; ++i)
	{
		m_patternsA[i] = 0x15'00'00'00;
		m_patternsB[i] = 0x15'00'00'00;
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

	memcpy(&m_displayedValues, &m_currentValues, sizeof(Measurement));

	emit measurementChanged();
}

void QFrameDetector::appendSettings(QByteArray *buffer)
{
	if(!buffer) return;

	quint8 enable = 0;

	for(int i = 0; i < 6; ++i)
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
	buffer->append(QByteArray((const char*) m_patternsA, PATTERN_MEM_SIZE * 4));

	buffer->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(buffer, MSG_ID_FD_PATTERNS);
	appendAsBytes<quint16>(buffer, PATTERN_MEM_SIZE * 4 + 1);

	appendAsBytes<quint8>(buffer, 1);
	buffer->append(QByteArray((const char*) m_patternsB, PATTERN_MEM_SIZE * 4));
}

void QFrameDetector::receiveMeasurement(const QByteArray &measurement)
{
	m_mutex.lock();

	m_currentValues.time = readAsNumber<quint64>(measurement, 0);
	m_currentValues.matched = readAsNumber<quint32>(measurement, 8);

	m_mutex.unlock();

	if(m_logFile.isWritable())
	{
		m_logFile.write(QByteArray::number(m_currentValues.time));
		m_logFile.putChar(',');
		m_logFile.write(QByteArray::number(m_currentValues.matched));
		m_logFile.putChar('\n');
	}
}

void QFrameDetector::loadPattern(quint32 id, QUrl url)
{
	if(id >= 6) return;

	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
		return;

	QFile patternFile;
	patternFile.setFileName(selectedPath);
	patternFile.open(QIODevice::ReadOnly);

	if(!patternFile.isOpen())
		return;

	m_patternPath[id] = selectedPath;

	quint32 *pattern = (id < 3 ? m_patternsA : m_patternsB);
	if(id >= 3) id -= 3;

	QByteArray fileContents = patternFile.readAll();
	uint32_t num = 0x10, i = 0;
	bool inX = false;

	for(char c : fileContents)
	{
		if(i >= PATTERN_MEM_SIZE) break;

		pattern[i] &= ~(0xFFu << 8*id);
		pattern[i] &= ~(0b11u << (24 + 2*id));

		if(inX)
		{
			inX = false;
			continue;
		}

		if(c == '$')
		{
			if(!i)
			{
				pattern[i++] |= 0x1u << (25 + 2*id);
			}
			else
			{
				pattern[i-1] |= 0x1u << (25 + 2*id);
			}

			break;
		}
		else if(c == 'x' || c == 'X')
		{
			inX = true;
			pattern[i++] |= 0x1u << (24 + 2*id);
		}
		else if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
		{
			c = tolower(c) - '0';

			if(c >= 10)
				c -= 'a' - '9' - 1;

			if(num <= 0xF)
			{
				pattern[i++] |= ((num << 4) | c) << 8*id;
				num = 0x10;
			}
			else
			{
				num = c;
			}
		}
	}

	for(; i < PATTERN_MEM_SIZE; ++i)
	{
		pattern[i] &= ~(0xFFu << 8*id);
		pattern[i] &= ~(0x1u << (25 + 2*id));
		pattern[i] |= 0x1u << (24 + 2*id);
	}

	emit patternsChanged();
}

void QFrameDetector::removePattern(quint32 id)
{
	if(id >= 6) return;

	m_patternPath[id] = "";

	if(id < 3)
	{
		for(int i = 0; i < PATTERN_MEM_SIZE; ++i)
		{
			m_patternsA[i] &= ~(0xFFu << 8*id);
			m_patternsA[i] &= ~(0x1u << (25 + 2*id));
			m_patternsA[i] |= 0x1u << (24 + 2*id);
		}
	}
	else
	{
		id -= 3;

		for(int i = 0; i < PATTERN_MEM_SIZE; ++i)
		{
			m_patternsB[i] &= ~(0xFFu << 8*id);
			m_patternsB[i] &= ~(0x1u << (25 + 2*id));
			m_patternsB[i] |= 0x1u << (24 + 2*id);
		}
	}

	emit patternsChanged();
}
