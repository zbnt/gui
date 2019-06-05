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

Item {
	id: root

	property var object: undefined

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		GroupBox {
			title: "Counters"
			topPadding: 26
			bottomPadding: 15

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
				rowSpacing: 4
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Ping-pongs:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: root.object.numPingPongs
					Layout.fillWidth: true
				}

				Label {
					text: "Lost pings:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: root.object.numLostPings
					Layout.fillWidth: true
				}

				Label {
					text: "Lost pongs:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: root.object.numLostPongs
					Layout.fillWidth: true
				}
			}
		}

		GroupBox {
			title: "Last measurement"
			topPadding: 26
			bottomPadding: 15

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
				rowSpacing: 4
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Ping time:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: root.object.lastPing
					Layout.fillWidth: true
				}

				Label {
					text: "Pong time:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: root.object.lastPong
					Layout.fillWidth: true
				}
			}
		}

		Item {
			Layout.fillHeight: true
		}
	}
}
