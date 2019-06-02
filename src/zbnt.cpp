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

#include <zbnt.hpp>

#include <vector>

#include <QFileDialog>
#include <QUrl>

ZBNT::ZBNT() : QObject(nullptr)
{
	m_socket = new QTcpSocket(this);

	connect(m_socket, &QTcpSocket::connected, this, &ZBNT::onConnected);
	connect(m_socket, &QTcpSocket::disconnected, this, &ZBNT::onDisconnected);
	connect(m_socket, &QTcpSocket::readyRead, this, &ZBNT::readSocket);
}

ZBNT::~ZBNT()
{
}

QString ZBNT::cyclesToTime(QString cycles)
{
	static const std::vector<std::pair<const char*, qulonglong>> convTable = {
		{"d", 24 * 60 * 60 * 1'000'000'000ull},
		{"h", 60 * 60 * 1'000'000'000ull},
		{"m", 60 * 1'000'000'000ull},
		{"s", 1'000'000'000ull},
		{"ms", 1'000'000},
		{"us", 1'000},
		{"ns", 1}
	};

	qulonglong cyclesInt = cycles.toULongLong() * 8;
	bool first = true;
	QString res;

	for(const auto &e : convTable)
	{
		qulonglong step = cyclesInt / e.second;

		if(step || (first && e.second == 1))
		{
			if(!first)
			{
				res += " + ";
			}

			first = false;

			res += QString::number(step);
			res += " ";
			res += e.first;
		}

		cyclesInt %= e.second;
	}

	return res;
}

void ZBNT::loadHeaders(int index, QUrl url)
{
	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
		return;

	QFile headersFile;
	headersFile.setFileName(selectedPath);
	headersFile.open(QIODevice::ReadOnly);

	if(!headersFile.isOpen())
		return;

	QByteArray *headersContents = index ? &m_headers2 : &m_headers1;

	if(selectedPath.endsWith(".hex"))
	{
		QByteArray fileContents = headersFile.readAll();
		uint8_t num = 0x10;

		headersContents->clear();

		for(char c : fileContents)
		{
			if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
			{
				c = tolower(c) - '0';

				if(c >= 10)
					c -= 'a' - '9' - 1;

				if(num <= 0xF)
				{
					headersContents->append((num << 4) | c);
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
		*headersContents = headersFile.readAll();
	}

	if(index)
	{
		m_headersLen2 = headersContents->length();
		m_headersPath2 = selectedPath;
		m_headersLoaded2 = true;

		emit headers2Changed();
	}
	else
	{
		m_headersLen1 = headersContents->length();
		m_headersPath1 = selectedPath;
		m_headersLoaded1 = true;

		emit headers1Changed();
	}
}

void ZBNT::onConnected()
{
}

void ZBNT::onDisconnected()
{
}

void ZBNT::readSocket()
{
}
