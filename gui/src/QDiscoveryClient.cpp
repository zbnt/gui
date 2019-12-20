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

#include <QDiscoveryClient.hpp>

#include <QDateTime>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include <Utils.hpp>

QDiscoveryClient::QDiscoveryClient(QObject *parent) : QObject(parent)
{
	m_client = new QUdpSocket(this);

	connect(m_client, &QUdpSocket::readyRead, this, &QDiscoveryClient::onReadyRead);
}

QDiscoveryClient::~QDiscoveryClient()
{ }

void QDiscoveryClient::findDevices()
{
	QByteArray message;

	m_time = QDateTime::currentDateTime().toSecsSinceEpoch();

	message.append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint16>(&message, MSG_ID_DISCOVERY);
	appendAsBytes<quint16>(&message, 8);
	appendAsBytes<quint64>(&message, m_time);

	for(const QNetworkInterface &iface : QNetworkInterface::allInterfaces())
	{
		if(iface.type() == QNetworkInterface::Loopback) continue;

		for(const QNetworkAddressEntry &address : iface.addressEntries())
		{
			m_client->writeDatagram(message, address.broadcast(), MSG_DISCOVERY_PORT);
		}
	}
}

quint64 QDiscoveryClient::scanTime()
{
	return m_time;
}

void QDiscoveryClient::onMessageReceived(quint16 id, const QByteArray &data)
{
	if(id == MSG_ID_DISCOVERY && data.length() > 36)
	{
		emit deviceDiscovered(data);
	}
}

void QDiscoveryClient::onReadyRead()
{
	while(m_client->hasPendingDatagrams())
	{
		QNetworkDatagram datagram = m_client->receiveDatagram();
		handleIncomingData(datagram.data());
	}
}
