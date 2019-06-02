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

#include <QTcpSocket>
#include <QUrl>

class ZBNT : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool running MEMBER m_running NOTIFY runningChanged)
	Q_PROPERTY(bool connected MEMBER m_connected NOTIFY connectedChanged)

	Q_PROPERTY(bool headersLoaded1 MEMBER m_headersLoaded1 NOTIFY headers1Changed)
	Q_PROPERTY(quint32 headersLen1 MEMBER m_headersLen1 NOTIFY headers1Changed)
	Q_PROPERTY(QString headersPath1 MEMBER m_headersPath1 NOTIFY headers1Changed)

	Q_PROPERTY(bool headersLoaded2 MEMBER m_headersLoaded2 NOTIFY headers2Changed)
	Q_PROPERTY(quint32 headersLen2 MEMBER m_headersLen2 NOTIFY headers2Changed)
	Q_PROPERTY(QString headersPath2 MEMBER m_headersPath2 NOTIFY headers2Changed)

public:
	ZBNT();
	~ZBNT();

public slots:
	QString cyclesToTime(QString cycles);
	void loadHeaders(int index, QUrl url);

signals:
	void runningChanged();
	void connectedChanged();
	void headers1Changed();
	void headers2Changed();

private slots:
	void onConnected();
	void onDisconnected();
	void readSocket();

private:
	QTcpSocket *m_socket = nullptr;
	QByteArray m_readBuffer;

	bool m_running = false;
	bool m_connected = false;

	// eth0
	bool m_headersLoaded1 = false;
	quint32 m_headersLen1 = 0;
	QString m_headersPath1;
	QByteArray m_headers1;

	// eth1
	bool m_headersLoaded2 = false;
	quint32 m_headersLen2 = 0;
	QString m_headersPath2;
	QByteArray m_headers2;
};
