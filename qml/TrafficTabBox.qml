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

GroupBox {
	id: root
	topPadding: 26
	bottomPadding: 15

	property string txBytes: "0"
	property string txGood: "0"
	property string txBad: "0"

	property string rxBytes: "0"
	property string rxGood: "0"
	property string rxBad: "0"

	label: Label {
		y: 5
		text: parent.title
		verticalAlignment: Text.AlignTop
		horizontalAlignment: Text.AlignHCenter
		anchors.horizontalCenter: parent.horizontalCenter
	}

	Layout.fillWidth: true

	GridLayout {
		columns: 2
		columnSpacing: 10

		anchors.fill: parent
		anchors.topMargin: 5
		anchors.leftMargin: 5
		anchors.rightMargin: 5

		GridLayout {
			columns: 2
			rowSpacing: 5
			columnSpacing: 10

			Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

			Label {
				text: "TX:"
				font.weight: Font.Bold
				Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			}

			Label {
				text: root.txBytes + " bytes"
			}

			Label {
				Layout.rowSpan: 2
			}

			Label {
				text: root.txGood + " good frames"
			}

			Label {
				text: root.txBad + " bad frames"
			}
		}

		GridLayout {
			columns: 2
			rowSpacing: 5
			columnSpacing: 10

			Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

			Label {
				text: "RX:"
				font.weight: Font.Bold
				Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			}

			Label {
				text: root.rxBytes + " bytes"
			}

			Label {
				Layout.rowSpan: 2
			}

			Label {
				text: root.rxGood + " good frames"
			}

			Label {
				text: root.rxBad + " bad frames"
			}
		}

		Item {
			Layout.fillHeight: true
			Layout.fillWidth: true
		}

		Item {
			Layout.fillHeight: true
			Layout.fillWidth: true
		}
	}
}
