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

#include <net/ZbntLocalClient.hpp>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include <MessageUtils.hpp>

ZbntLocalClient::ZbntLocalClient(qint64 pid, QObject *parent)
	: ZbntClient(parent), m_pid(pid)
{
	m_socket = new QLocalSocket(this);

	connect(m_socket, &QLocalSocket::readyRead, this, &ZbntLocalClient::onReadyRead);
	connect(m_socket, &QLocalSocket::stateChanged, this, &ZbntLocalClient::onStateChanged);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	connect(m_socket, &QLocalSocket::errorOccurred, this, &ZbntLocalClient::onErrorOccurred);
#else
	connect(m_socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &ZbntLocalClient::onErrorOccurred);
#endif
}

ZbntLocalClient::~ZbntLocalClient()
{ }

void ZbntLocalClient::connectToServer()
{
#ifdef __linux__
	if(m_socket->state() != QLocalSocket::UnconnectedState)
		return;

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if(sock == -1)
	{
		emit connectionError("Socket error");
		return;
	}

	sockaddr_un sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	snprintf(sockAddr.sun_path + 1, sizeof(sockAddr.sun_path) - 1, "/tmp/zbnt-local-%016llX", m_pid);
	sockAddr.sun_family = AF_UNIX;

	if(::connect(sock, (sockaddr*) &sockAddr, sizeof(sa_family_t) + 33) == -1)
	{
		emit connectionError("Can't connect to server");
		return;
	}

	if(!m_socket->setSocketDescriptor(sock))
	{
		emit connectionError("Invalid socket descriptor");
	}

	emit stateChanged(Connected);
#else
	emit connectionError("Client not supported");
#endif
}

void ZbntLocalClient::closeConnection()
{
#ifdef __linux__
	m_socket->close();
#endif
}

void ZbntLocalClient::abortConnection()
{
#ifdef __linux__
	m_socket->abort();
#endif
}

void ZbntLocalClient::write(const QByteArray &data)
{
#ifdef __linux__
	m_socket->write(data);
#endif
}

void ZbntLocalClient::writeMessage(MessageID msgID, const QByteArray &value)
{
#ifdef __linux__
	::writeMessage(m_socket, msgID, value);
#endif
}

void ZbntLocalClient::onReadyRead()
{
#ifdef __linux__
	handleIncomingData(m_socket->readAll());
#endif
}

void ZbntLocalClient::onStateChanged(QLocalSocket::LocalSocketState state)
{
	switch(state)
	{
		case QLocalSocket::UnconnectedState:
		{
			emit stateChanged(Disconnected);
			return;
		}

		case QLocalSocket::ConnectingState:
		{
			emit stateChanged(Connecting);
			break;
		}

		case QLocalSocket::ConnectedState:
		{
			emit stateChanged(Connected);
			break;
		}

		default: { }
	}
}

void ZbntLocalClient::onErrorOccurred(QLocalSocket::LocalSocketError error)
{
	switch(error)
	{
		case QLocalSocket::ServerNotFoundError:
		{
			emit connectionError("Server not found");
			break;
		}

		case QLocalSocket::SocketTimeoutError:
		{
			emit connectionError("Timeout");
			break;
		}

		case QLocalSocket::ConnectionRefusedError:
		{
			emit connectionError("Connection refused");
			break;
		}

		default:
		{
			emit connectionError("Unknown error");
			break;
		}
	}
}
