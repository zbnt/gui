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

import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import zbnt 1.0

GridLayout {
	id: root

	columns: 2
	columnSpacing: 10

	property var object: undefined

	Label {
		text: "TX Rate:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.txRate + "/s"
		Layout.fillWidth: true
	}

	Label {
		text: "TX Bytes:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.txBytes + " (" + ZBNT.bytesToHumanReadable(root.object.txBytes) + ")"
		Layout.fillWidth: true
	}

	Label {
		text: "TX Good:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.txGood
		Layout.fillWidth: true
	}

	Label {
		text: "TX Bad:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.txBad
		Layout.fillWidth: true
	}

	Item {
		Layout.minimumHeight: 6
		Layout.minimumWidth: parent.width / 2
	}

	Item {
		Layout.minimumHeight: 6
		Layout.fillWidth: true
	}

	Label {
		text: "RX Rate:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.rxRate + "/s"
		Layout.fillWidth: true
	}

	Label {
		text: "RX Bytes:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.rxBytes + " (" + ZBNT.bytesToHumanReadable(root.object.rxBytes) + ")"
		Layout.fillWidth: true
	}

	Label {
		text: "RX Good:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.rxGood
		Layout.fillWidth: true
	}

	Label {
		text: "RX Bad:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	Label {
		text: root.object.rxBad
		Layout.fillWidth: true
	}

	Item {
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.columnSpan: 2
	}
}
