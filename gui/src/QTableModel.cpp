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

#include <QTableModel.hpp>

QTableModel::QTableModel(QObject *parent)
	: QAbstractTableModel(parent), m_numColumns(1)
{
}

QTableModel::QTableModel(quint16 columns, QObject *parent)
	: QAbstractTableModel(parent), m_numColumns(columns)
{ }

int QTableModel::rowCount(const QModelIndex&) const
{
	return m_items.size();
}

int QTableModel::columnCount(const QModelIndex&) const
{
	return m_numColumns;
}

QVariant QTableModel::data(const QModelIndex &idx, int role) const
{
	if(role == Qt::DisplayRole)
	{
		if(idx.row() < m_items.size())
		{
			const auto &row = m_items[idx.row()];

			if(idx.column() < row.size())
			{
				return row[idx.column()];
			}
		}
	}

	return QVariant();
}

QHash<int, QByteArray> QTableModel::roleNames() const
{
	return { {Qt::DisplayRole, "display"} };
}

void QTableModel::prependRows(const QList<QStringList> &rows)
{
	if(!rows.size()) return;

	beginInsertRows(QModelIndex(), 0, rows.size() - 1);

	for(const QStringList &r : rows)
	{
		m_items.prepend(r);
	}

	endInsertRows();
}

void QTableModel::removeRows(quint16 start, quint16 end)
{
	if(start >= end || end > m_items.size()) return;

	beginRemoveRows(QModelIndex(), start, end);
	m_items.erase(m_items.begin() + start, m_items.begin() + end);
	endRemoveRows();
}

void QTableModel::clearRows()
{
	beginResetModel();
	m_items.clear();
	endResetModel();
}
