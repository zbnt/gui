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

#pragma once

#include <QTimer>
#include <QUdpSocket>
#include <Messages.hpp>
#include <MessageReceiver.hpp>

class QDiscoveryClient : public QObject
{
	Q_OBJECT

public:
	QDiscoveryClient(QObject *parent = nullptr);
	~QDiscoveryClient();

	void findDevices();
	quint64 validator();

	void onReadyRead();

signals:
	void discoveryTimeout();
	void deviceDiscovered(const QHostAddress &addr, const QByteArray &data);

private:
	quint64 m_validator = 0;
	QTimer *m_timer = nullptr;
	QUdpSocket *m_client = nullptr;
};
