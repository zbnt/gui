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

#pragma once

#include <QUdpSocket>
#include <Messages.hpp>
#include <MessageReceiver.hpp>

class QDiscoveryClient : public QObject, public MessageReceiver
{
	Q_OBJECT

public:
	QDiscoveryClient(QObject *parent = nullptr);
	~QDiscoveryClient();

	void findDevices();
	quint64 scanTime();

	void onMessageReceived(quint8 id, const QByteArray &data);
	void onReadyRead();

signals:
	void deviceDiscovered(const QByteArray &data);

private:
	quint64 m_time = 0;
	QUdpSocket *m_client = nullptr;
};
