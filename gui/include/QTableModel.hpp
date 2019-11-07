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

#include <QAbstractTableModel>
#include <QHash>
#include <QList>

class QTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	QTableModel(QObject *parent = nullptr);
	QTableModel(quint16 columns, QObject *parent = nullptr);

	int rowCount(const QModelIndex& = QModelIndex()) const override;
	int columnCount(const QModelIndex& = QModelIndex()) const override;

	QVariant data(const QModelIndex &idx, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	void prependRows(const QList<QStringList> &rows);
	void erase(quint16 start, quint16 end);

private:
	QList<QStringList> m_items;
	quint16 m_numColumns;
};
