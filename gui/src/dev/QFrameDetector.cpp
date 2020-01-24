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

	m_patternsEnabled = 0;
	m_detectionListA = new QTableModel(3, this);
	m_detectionListB = new QTableModel(3, this);
}

QFrameDetector::~QFrameDetector()
{ }

void QFrameDetector::setExtraInfo(quint64 values)
{
	m_ports = values & 0xFFFF;
	m_numPatterns = (values >> 16) & 0xFFFF;
	m_maxPatternLength = (values >> 32) & 0xFFFF;

	for(int i = 0; i < m_numPatterns*2; ++i)
	{
		m_patternPath.append("");
		m_patternBytes.append(QByteArray());
		m_patternFlags.append(QByteArray());
	}
}

void QFrameDetector::enableLogging(const QString &path)
{
	disableLogging();

	m_logFile.setFileName(path + QString("/eth%1_eth%2_detector.csv").arg(m_ports & 0xFF).arg((m_ports >> 8) & 0xFF));
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

QString QFrameDetector::settingsQml() const
{
	return QString("qrc:/qml/SettingsTabFD.qml");
}

QString QFrameDetector::statusQml() const
{
	return QString("qrc:/qml/StatusTabFD.qml");
}

quint32 QFrameDetector::portA() const
{
	return m_ports & 0xFF;
}

quint32 QFrameDetector::portB() const
{
	return (m_ports >> 8) & 0xFF;
}

bool QFrameDetector::loadPattern(quint32 id, QUrl url)
{
	if(id >= m_numPatterns*2)
	{
		emit error("Invalid pattern ID");
		return false;
	}

	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
	{
		emit error("No file selected");
		return false;
	}

	QFile patternFile;
	patternFile.setFileName(selectedPath);
	patternFile.open(QIODevice::ReadOnly);

	if(!patternFile.isOpen())
	{
		emit error("Can't read pattern file, make sure you have the required permissions");
		return false;
	}

	QByteArray patternBytes(m_maxPatternLength + 2, '\0');
	QByteArray patternFlags(m_maxPatternLength + 2, '\0');
	QByteArray fileContents = patternFile.readAll();
	quint32 num = 0x10;
	bool inX = false;
	int i = 2;

	patternBytes[0] = (id >= m_numPatterns);
	patternBytes[1] = (id % m_numPatterns);
	patternFlags[0] = patternBytes[0];
	patternFlags[1] = patternBytes[1];

	for(char c : fileContents)
	{
		if(i >= m_maxPatternLength + 2)
		{
			emit error(QString("Pattern can not be larger than %1 bytes").arg(m_maxPatternLength));
			return false;
		}

		if(inX)
		{
			inX = false;
			continue;
		}

		if(c == '$')
		{
			if(i < 1)
			{
				patternFlags[i] = patternFlags[i] | 0x2;
				i++;
			}
			else
			{
				patternFlags[i-1] = patternFlags[i-1] | 0x2;
			}

			break;
		}
		else if(c == '!')
		{
			if(i >= 1)
			{
				patternFlags[i-1] = patternFlags[i-1] | 0x5;
			}
		}
		else if(c == '+')
		{
			if(i >= 1)
			{
				if((patternFlags[i-1] & 0x60) == 0x20)
				{
					patternFlags[i-1] = patternFlags[i-1] | 0x41;
					patternFlags[i-1] = patternFlags[i-1] & ~0x20;
				}
				else if(!(patternFlags[i-1] & 0x60))
				{
					patternFlags[i-1] = patternFlags[i-1] | 0x21;
				}
			}
		}
		else if(c == '?')
		{
			inX = true;
			patternFlags[i] = patternFlags[i] | 0x11;
			i++;
		}
		else if(c == 'r' || c == 'R')
		{
			inX = true;
			patternFlags[i] = patternFlags[i] | 0x9;
			i++;
		}
		else if(c == 'x' || c == 'X')
		{
			inX = true;
			patternFlags[i] = patternFlags[i] | 0x1;
			i++;
		}
		else if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
		{
			c = tolower(c) - '0';

			if(c >= 10)
			{
				c -= 'a' - '9' - 1;
			}

			if(num <= 0xF)
			{
				patternBytes[i] = (num << 4) | c;
				num = 0x10;
				i++;
			}
			else
			{
				num = c;
			}
		}
	}

	for(; i < m_maxPatternLength + 2; i++)
	{
		patternFlags[i] = 1;
	}

	m_patternPath[id] = selectedPath;
	m_patternBytes[id] = patternBytes;
	m_patternFlags[id] = patternFlags;
	m_patternsEnabled |= (1 << id);
	emit patternsChanged();

	return true;
}

void QFrameDetector::removePattern(quint32 id)
{
	if(id >= m_numPatterns*2) return;

	QByteArray patternData;
	patternData.append(id >= m_numPatterns);
	patternData.append(id % m_numPatterns);

	m_patternPath[id] = "";
	m_patternBytes[id] = patternData;
	m_patternFlags[id] = patternData;
	m_patternsEnabled &= ~(1 << id);
	emit patternsChanged();
}
