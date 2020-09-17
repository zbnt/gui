/*
	zbnt/gui
	Copyright (C) 2020 Oscar R.

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

#pragma once

#include <QHostAddress>
#include <QTcpSocket>

#include <net/ZbntClient.hpp>

class ZbntTcpClient : public ZbntClient
{
public:
	ZbntTcpClient(const QString &ip, quint16 port, QObject *parent = nullptr);
	~ZbntTcpClient();

	void connectToServer();
	void closeConnection();
	void abortConnection();

	void write(const QByteArray &data);
	void writeMessage(MessageID msgID, const QByteArray &value);

private:
	void onReadyRead();
	void onStateChanged(QAbstractSocket::SocketState state);
	void onErrorOccurred(QAbstractSocket::SocketError error);

private:
	QTcpSocket *m_socket = nullptr;
	QHostAddress m_address;
	quint16 m_port;
};
