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

#include <dev/QAbstractDevice.hpp>

QAbstractDevice::QAbstractDevice(DeviceType type, QObject *parent)
	: QObject(parent), m_id(0), m_type(type)
{ }

QAbstractDevice::~QAbstractDevice()
{ }

quint8 QAbstractDevice::id() const
{
	return m_id;
}

void QAbstractDevice::setID(quint8 id)
{
	m_id = id;
}

DeviceType QAbstractDevice::type() const
{
	return m_type;
}

quint32 QAbstractDevice::setPcapOutput(std::shared_ptr<QIODevice>&, quint32)
{
	return 0;
}

void QAbstractDevice::enableLogging(const QString&)
{ }

void QAbstractDevice::disableLogging()
{ }
