/*
	zbnt_gui
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

import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import zbnt 1.0

RowLayout {
	id: root
	spacing: 2

	property bool available: false
	property string text: ""
	property int iconSize: 14

	Label {
		text: root.available ? "\uE876" : "\uE5CD"
		font.family: "Material Icons"
		font.pointSize: root.iconSize
		color: root.available ? "#43a047" : "#e53935"
	}

	Label {
		text: root.text
	}
}

