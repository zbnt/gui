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

#include <QString>
#include <QAbstractSocket>

#include <Messages.hpp>

extern void cyclesToTime(quint64 cycles, QString &time);
extern void bytesToHumanReadable(quint64 bytes, QString &res, bool dec = false);
extern void bitsToHumanReadable(quint64 bytes, QString &res, bool dec = false);

template<typename T>
void sendAsBytes(QAbstractSocket *socket, T data)
{
	socket->write((const char*) &data, sizeof(T));
}

template<typename T>
void appendAsBytes(QByteArray *array, T data)
{
	array->append((const char*) &data, sizeof(T));
}

template<typename T>
void setDeviceProperty(QByteArray *array, quint8 devID, PropertyID propID, T value)
{
	array->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint16>(array, MSG_ID_SET_PROPERTY);
	appendAsBytes<quint16>(array, 3 + sizeof(T));
	appendAsBytes<quint8>(array, devID);
	appendAsBytes<quint16>(array, propID);
	appendAsBytes(array, value);
}

extern void setDeviceProperty(QByteArray *array, quint8 devID, PropertyID propID, const QByteArray &value);

template<typename T>
T readAsNumber(const QByteArray &data, quint32 offset)
{
	T res = 0;

	for(uint i = 0; i < sizeof(T); ++i)
	{
		res |= T(quint8(data[offset + i])) << (8 * i);
	}

	return res;
}
