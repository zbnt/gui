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

#include <QObject>

#include <MessageReceiver.hpp>

class ZbntClient : public QObject, public MessageReceiver
{
	Q_OBJECT

public:
	enum State
	{
		Disconnected,
		Connecting,
		Connected
	};

	Q_ENUM(State)

public:
	ZbntClient(QObject *parent = nullptr);
	~ZbntClient();

	virtual void connectToServer() = 0;
	virtual void closeConnection() = 0;
	virtual void abortConnection() = 0;

	virtual void write(const QByteArray &data) = 0;
	virtual void writeMessage(MessageID msgID, const QByteArray &value) = 0;

protected:
	void onMessageReceived(quint16 id, const QByteArray &data);

signals:
	void stateChanged(State state);
	void connectionError(const char *error);
	void messageReceived(quint16 id, const QByteArray &data);
};
