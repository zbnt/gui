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

#include <net/ZbntTcpClient.hpp>

#include <MessageUtils.hpp>

ZbntTcpClient::ZbntTcpClient(const QString &ip, quint16 port, QObject *parent)
	: ZbntClient(parent), m_address(ip), m_port(port)
{
	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	connect(m_socket, &QTcpSocket::readyRead, this, &ZbntTcpClient::onReadyRead);
	connect(m_socket, &QTcpSocket::stateChanged, this, &ZbntTcpClient::onStateChanged);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	connect(m_socket, &QTcpSocket::errorOccurred, this, &ZbntTcpClient::onErrorOccurred);
#else
	connect(m_socket, QOverload<QTcpSocket::SocketError>::of(&QTcpSocket::error), this, &ZbntTcpClient::onErrorOccurred);
#endif
}

ZbntTcpClient::~ZbntTcpClient()
{ }

void ZbntTcpClient::connectToServer()
{
	m_socket->connectToHost(m_address, m_port);
}

void ZbntTcpClient::closeConnection()
{
	m_socket->close();
}

void ZbntTcpClient::abortConnection()
{
	m_socket->abort();
}

void ZbntTcpClient::write(const QByteArray &data)
{
	m_socket->write(data);
}

void ZbntTcpClient::writeMessage(MessageID msgID, const QByteArray &value)
{
	::writeMessage(m_socket, msgID, value);
}

void ZbntTcpClient::onReadyRead()
{
	handleIncomingData(m_socket->readAll());
}

void ZbntTcpClient::onStateChanged(QAbstractSocket::SocketState state)
{
	switch(state)
	{
		case QTcpSocket::UnconnectedState:
		{
			emit stateChanged(Disconnected);
			return;
		}

		case QTcpSocket::ConnectingState:
		{
			emit stateChanged(Connecting);
			break;
		}

		case QTcpSocket::ConnectedState:
		{
			emit stateChanged(Connected);
			break;
		}

		default: { }
	}
}

void ZbntTcpClient::onErrorOccurred(QAbstractSocket::SocketError error)
{
	switch(error)
	{
		case QAbstractSocket::NetworkError:
		{
			emit connectionError("Unreachable address");
			break;
		}

		case QAbstractSocket::SocketTimeoutError:
		{
			emit connectionError("Timeout");
			break;
		}

		case QAbstractSocket::ConnectionRefusedError:
		{
			emit connectionError("Connection refused");
			break;
		}

		default:
		{
			emit connectionError("Unknown error");
		}
	}
}

