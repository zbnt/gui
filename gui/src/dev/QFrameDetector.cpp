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

#include <Utils.hpp>
#include <Messages.hpp>

QFrameDetector::QFrameDetector(QObject *parent)
	: QAbstractDevice(parent)
{
	for(int i = 0; i < 8; ++i)
	{
		m_patternPath.append("");
		m_detectionCounters.append(0);
		m_detectionCountersStr.append("0");
	}

	for(int i = 0; i < PATTERN_MEM_SIZE; ++i)
	{
		m_patternDataA[i] = 0;
		m_patternDataB[i] = 0;

		m_patternFlagsA[i] = 1;
		m_patternFlagsB[i] = 1;
	}

	m_detectionListA = new QTableModel(3, this);
	m_detectionListB = new QTableModel(3, this);
}

QFrameDetector::~QFrameDetector()
{ }

void QFrameDetector::enableLogging(const QString &path)
{
	disableLogging();

	m_logFile.setFileName(path); // TODO
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

	m_mutex.unlock();

	if(m_detectionListA->rowCount() > 1000)
	{
		m_detectionListA->removeRows(1000, m_detectionListA->rowCount());
	}

	if(m_detectionListB->rowCount() > 1000)
	{
		m_detectionListB->removeRows(1000, m_detectionListB->rowCount());
	}

	emit measurementChanged();
}

void QFrameDetector::appendSettings(QByteArray &buffer)
{
	quint32 enable = 0;

	for(int i = 0; i < 8; ++i)
	{
		if(m_patternPath[i].toString().length())
		{
			if(i < 4)
			{
				enable |= 1 << i;
			}
			else
			{
				enable |= 1 << (i + 12);
			}
		}
	}

	setDeviceProperty<quint8>(buffer, 6, PROP_ENABLE_CSUM_FIX, m_fixChecksums);
	setDeviceProperty<quint32>(buffer, 6, PROP_ENABLE_PATTERN, enable);
	setDeviceProperty<quint8>(buffer, 6, PROP_ENABLE, !!enable);

	m_mutex.lock();

	m_pendingDetections[0].clear();
	m_pendingDetections[1].clear();
	m_detectionListA->clearRows();
	m_detectionListB->clearRows();

	for(int i = 0; i < 8; ++i)
	{
		m_detectionCounters[i] = 0;
		m_detectionCountersStr[i] = "0";
	}

	m_mutex.unlock();

	emit measurementChanged();
}

/*void QFrameDetector::appendPatterns(QByteArray &buffer)
{
	for(const quint8 *ptr : {m_patternDataA, m_patternDataB})
	{
		for(int idx = 0; idx < 4; ++idx)
		{
			QByteArray msg;
			appendAsBytes<quint8>(msg, ptr == m_patternDataB);
			appendAsBytes<quint8>(msg, idx);

			for(int j = idx; j < PATTERN_MEM_SIZE; j += 4)
			{
				msg.append(ptr[j]);
			}

			setDeviceProperty(buffer, 6, PROP_FRAME_PATTERN, msg);
		}
	}

	for(const quint8 *ptr : {m_patternFlagsA, m_patternFlagsB})
	{
		for(int idx = 0; idx < 4; ++idx)
		{
			QByteArray msg;
			appendAsBytes<quint8>(msg, ptr == m_patternFlagsB);
			appendAsBytes<quint8>(msg, idx);

			for(int j = idx; j < PATTERN_MEM_SIZE; j += 4)
			{
				msg.append(ptr[j]);
			}

			setDeviceProperty(buffer, 6, PROP_FRAME_PATTERN_FLAGS, msg);
		}
	}
}*/ // TODO

void QFrameDetector::receiveMeasurement(const QByteArray &measurement)
{
	if(measurement.size() < 12) return;

	quint64 time = readAsNumber<quint64>(measurement, 0);
	quint8 match_dir = readAsNumber<quint8>(measurement, 8);
	quint8 match_mask = readAsNumber<quint8>(measurement, 9);
	quint16 ext_num = readAsNumber<quint16>(measurement, 10);
	QByteArray match_ext_data = measurement.mid(12, ext_num).toHex();

	if(ext_num > 16)
		ext_num = 16;

	match_dir = (match_dir == 'A');

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
{ }

QString QFrameDetector::description() const
{
	return QString("Frame detector (eth%1, eth%2)").arg(m_ports & 0xFF).arg((m_ports >> 8) & 0xFF);
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
	quint32 num = 0x10, i = id;
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
		else if(c == '+')
		{
			if(i >= 4)
			{
				if((patternFlags[i-4] & 0x60) == 0x20)
				{
					patternFlags[i-4] |= 0x41;
					patternFlags[i-4] &= ~0x20;
				}
				else if(!(patternFlags[i-4] & 0x60))
				{
					patternFlags[i-4] |= 0x21;
				}
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
