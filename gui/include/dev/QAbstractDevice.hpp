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

#include <QObject>
#include <QList>
#include <QPair>

#include <Messages.hpp>

struct BitstreamDevInfo
{
	quint8 id;
	DeviceType type;
	QList<QPair<PropertyID, QByteArray>> properties;
};

class QAbstractDevice : public QObject
{
	Q_OBJECT

public:
	QAbstractDevice(QObject *parent = nullptr);
	~QAbstractDevice();

	virtual void loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props) = 0;

	virtual void enableLogging(const QString &path) = 0;
	virtual void disableLogging() = 0;

	virtual void updateDisplayedValues() = 0;

	virtual void receiveMeasurement(const QByteArray &measurement) = 0;
	virtual void resetMeasurement() = 0;

public slots:
	virtual QString description() const = 0;
	virtual QString settingsQml() const = 0;
	virtual QString statusQml() const = 0;
};
