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

void QTrafficGenerator::sendSettings(QTcpSocket *socket)
{
	if(!socket) return;

	socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(socket, MSG_ID_TG_CFG);
	sendAsBytes<quint16>(socket, 28);

	sendAsBytes<quint8>(socket, m_idx);
	sendAsBytes<quint8>(socket, m_enable);

	sendAsBytes<quint8>(socket, m_paddingMethod);
	sendAsBytes<quint16>(socket, m_paddingConstant);
	sendAsBytes<quint16>(socket, m_paddingRangeBottom);
	sendAsBytes<quint16>(socket, m_paddingRangeTop);
	sendAsBytes<quint16>(socket, m_paddingAverage);

	sendAsBytes<quint8>(socket, m_delayMethod);
	sendAsBytes<quint32>(socket, m_delayConstant.toULong());
	sendAsBytes<quint32>(socket, m_delayRangeBottom.toULong());
	sendAsBytes<quint32>(socket, m_delayRangeTop.toULong());
	sendAsBytes<quint32>(socket, m_delayAverage.toULong());
}

void QTrafficGenerator::sendHeaders(QTcpSocket *socket)
{
	if(!socket) return;
	if(!m_enable) return;

	socket->write(MSG_MAGIC_IDENTIFIER, 4);
	sendAsBytes<quint8>(socket, MSG_ID_TG_HEADERS);
	sendAsBytes<quint16>(socket, m_headersLength + 1);

	sendAsBytes<quint8>(socket, m_idx);
	socket->write(m_headers);
}

void QTrafficGenerator::loadHeaders(QUrl url)
{
	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
		return;

	QFile headersFile;
	headersFile.setFileName(selectedPath);
	headersFile.open(QIODevice::ReadOnly);

	if(!headersFile.isOpen())
		return;

	if(selectedPath.endsWith(".hex"))
	{
		QByteArray fileContents = headersFile.readAll();
		uint8_t num = 0x10;

		m_headers.clear();

		for(char c : fileContents)
		{
			if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
			{
				c = tolower(c) - '0';

				if(c >= 10)
					c -= 'a' - '9' - 1;

				if(num <= 0xF)
				{
					m_headers.append((num << 4) | c);
					num = 0x10;
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
		m_headers = headersFile.readAll();
	}

	m_headersLength = m_headers.length();
	m_headersPath = selectedPath;
	m_headersLoaded = true;

	emit headersChanged();
}
