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

#include <dev/QTrafficGenerator.hpp>

#include <QFile>

#include <Utils.hpp>
#include <Messages.hpp>

QTrafficGenerator::QTrafficGenerator(QObject *parent)
	: QAbstractDevice(parent)
{ }

QTrafficGenerator::~QTrafficGenerator()
{ }

void QTrafficGenerator::enableLogging(const QString &path)
{
	Q_UNUSED(path);
}

void QTrafficGenerator::disableLogging()
{ }

void QTrafficGenerator::updateDisplayedValues()
{ }

void QTrafficGenerator::appendSettings(QByteArray &buffer)
{
	setDeviceProperty(buffer, 4 + m_idx, PROP_FRAME_SIZE, m_frameSize.toUShort());
	setDeviceProperty(buffer, 4 + m_idx, PROP_FRAME_GAP, m_frameSize.toUInt());

	setDeviceProperty(buffer, 4 + m_idx, PROP_BURST_TIME_ON, m_burstOnTime.toULong());
	setDeviceProperty(buffer, 4 + m_idx, PROP_BURST_TIME_OFF, m_burstOffTime.toULong());
	setDeviceProperty(buffer, 4 + m_idx, PROP_ENABLE_BURST, m_burstEnable);

	setDeviceProperty(buffer, 4 + m_idx, PROP_LFSR_SEED, m_lfsrSeed.toUInt());

	setDeviceProperty(buffer, 4 + m_idx, PROP_ENABLE, m_enable);
}

void QTrafficGenerator::appendFrame(QByteArray &buffer)
{
	setDeviceProperty(buffer, 4 + m_idx, PROP_FRAME_TEMPLATE, m_templateBytes);
	setDeviceProperty(buffer, 4 + m_idx, PROP_FRAME_PATTERN, QByteArray((const char*) m_templateMask, 256));
}

void QTrafficGenerator::receiveMeasurement(const QByteArray &measurement)
{
	Q_UNUSED(measurement);
}

void QTrafficGenerator::resetMeasurement()
{ }

QString QTrafficGenerator::description() const
{
	return QString("Traffic generator (eth%1)").arg(m_ports & 0xFF);
}

void QTrafficGenerator::loadTemplate(QUrl url)
{
	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
		return;

	QFile headersFile;
	headersFile.setFileName(selectedPath);
	headersFile.open(QIODevice::ReadOnly);

	if(!headersFile.isOpen())
		return;

	memset(m_templateMask, 0xFF, sizeof(m_templateMask));

	if(selectedPath.endsWith(".hex"))
	{
		QByteArray fileContents = headersFile.readAll();
		quint8 num = 0x10, maskCount = 0;
		quint16 maskOffset = 0;
		bool inX = false;

		m_templateBytes.clear();

		for(char c : fileContents)
		{
			if(inX)
			{
				inX = false;
				continue;
			}

			if(c == 'x' || c == 'X')
			{
				inX = true;
				m_templateBytes.append('\0');

				++maskCount;

				if(maskCount >= 8)
				{
					maskCount = 0;
					++maskOffset;
				}
			}
			else if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
			{
				c = tolower(c) - '0';

				if(c >= 10)
					c -= 'a' - '9' - 1;

				if(num <= 0xF)
				{
					m_templateBytes.append((num << 4) | c);
					num = 0x10;

					if(maskOffset <= 255)
					{
						m_templateMask[maskOffset] &= ~(1u << maskCount);
						++maskCount;

						if(maskCount >= 8)
						{
							maskCount = 0;
							++maskOffset;
						}
					}
				}
				else
				{
					num = c;
				}
			}
		}
	}
	else
	{
		m_templateBytes = headersFile.readAll();
	}

	m_templateLength = m_templateBytes.length();
	m_templatePath = selectedPath;
	m_templateLoaded = true;

	emit templateChanged();
}


