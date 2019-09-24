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

#include <QTrafficGenerator.hpp>

#include <QFile>

#include <Utils.hpp>
#include <Messages.hpp>

QTrafficGenerator::QTrafficGenerator(QObject *parent) : QObject(parent)
{ }

QTrafficGenerator::~QTrafficGenerator()
{ }

void QTrafficGenerator::setIndex(quint8 idx)
{
	m_idx = idx;
}

void QTrafficGenerator::appendSettings(QByteArray *buffer)
{
	if(!buffer) return;

	buffer->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(buffer, MSG_ID_TG_CFG);
	appendAsBytes<quint16>(buffer, 21);

	appendAsBytes<quint8>(buffer, m_idx);
	appendAsBytes<quint8>(buffer, m_enable);

	appendAsBytes<quint16>(buffer, m_frameSize.toUShort());
	appendAsBytes<quint32>(buffer, m_frameDelay.toUInt());

	appendAsBytes<quint8>(buffer, m_burstEnable);
	appendAsBytes<quint16>(buffer, m_burstOnTime.toULong());
	appendAsBytes<quint16>(buffer, m_burstOffTime.toULong());

	appendAsBytes<quint64>(buffer, m_lfsrSeed.toULongLong());
}

void QTrafficGenerator::appendFrame(QByteArray *buffer)
{
	if(!buffer) return;

	buffer->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(buffer, MSG_ID_TG_FRAME);
	appendAsBytes<quint16>(buffer, m_templateLength + 1);

	appendAsBytes<quint8>(buffer, m_idx);
	buffer->append(m_templateBytes);

	buffer->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint8>(buffer, MSG_ID_TG_PATTERN);
	appendAsBytes<quint16>(buffer, 257);

	appendAsBytes<quint8>(buffer, m_idx);
	buffer->append((const char*) m_templateMask, 256);
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

	memset(m_templateMask, 0, sizeof(m_templateMask));

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

				if(maskOffset <= 255)
				{
					m_templateMask[maskOffset] |= 1u << maskCount;
					++maskCount;

					if(maskCount >= 8)
					{
						maskCount = 0;
						++maskOffset;
					}
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
