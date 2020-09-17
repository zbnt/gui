/*
	zbnt/gui
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

#include <QRandomGenerator>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include <MessageUtils.hpp>

QDiscoveryClient::QDiscoveryClient(QObject *parent) : QObject(parent)
{
	m_client = new QUdpSocket(this);
	m_timer = new QTimer(this);

	m_timer->setSingleShot(true);
	m_timer->setInterval(2000);

	connect(m_client, &QUdpSocket::readyRead, this, &QDiscoveryClient::onReadyRead);
	connect(m_timer, &QTimer::timeout, this, &QDiscoveryClient::discoveryTimeout);
}

QDiscoveryClient::~QDiscoveryClient()
{ }

void QDiscoveryClient::findDevices()
{
	QByteArray message;

	m_validator = QRandomGenerator::system()->generate64();

	message.append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint16>(message, MSG_ID_DISCOVERY);
	appendAsBytes<quint16>(message, 8);
	appendAsBytes<quint64>(message, m_validator);

	m_client->writeDatagram(message, QHostAddress::LocalHost, MSG_DISCOVERY_PORT);

	for(const QNetworkInterface &iface : QNetworkInterface::allInterfaces())
	{
		if(iface.type() == QNetworkInterface::Loopback) continue;

		for(const QNetworkAddressEntry &address : iface.addressEntries())
		{
			m_client->writeDatagram(message, address.broadcast(), MSG_DISCOVERY_PORT);
		}

		QHostAddress multicastAddr;
		multicastAddr.setAddress(QString("ff12::%1").arg(MSG_DISCOVERY_PORT));
		multicastAddr.setScopeId(iface.name());

		m_client->writeDatagram(message, multicastAddr, MSG_DISCOVERY_PORT);
	}

	m_timer->start();
}

quint64 QDiscoveryClient::validator()
{
	return m_validator;
}

void QDiscoveryClient::onReadyRead()
{
	while(m_client->hasPendingDatagrams())
	{
		QNetworkDatagram datagram = m_client->receiveDatagram();
		QByteArray response = datagram.data();

		if(!m_timer->isActive())
		{
			continue;
		}

		if(response.size() <= 54)
		{
			continue;
		}

		if(!response.startsWith(MSG_MAGIC_IDENTIFIER))
		{
			continue;
		}

		uint16_t messageID = readAsNumber<uint16_t>(response, 4);
		uint16_t messageSize = readAsNumber<uint16_t>(response, 6);

		if(messageID != MSG_ID_DISCOVERY || messageSize <= 54)
		{
			continue;
		}

		emit deviceDiscovered(datagram.senderAddress(), response.mid(8, messageSize));
	}
}
