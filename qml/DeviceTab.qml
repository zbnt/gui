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

Item {
	id: root

	property bool settingsValid: false

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		GroupBox {
			title: "Connection"
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
				rowSpacing: 5
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "IP Address:"
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					Layout.fillWidth: true
				}

				Item {
					Layout.fillHeight: true
				}

				RowLayout {
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

					Button {
						text: "Autodetect"
						enabled: !ZBNT.connected
						focusPolicy: Qt.NoFocus

						Layout.alignment: Qt.AlignRight

						onPressed: {
						}
					}

					Button {
						text: ZBNT.connected ? "Disconnect" : "Connect"
						enabled: !ZBNT.running
						focusPolicy: Qt.NoFocus

						Layout.alignment: Qt.AlignRight

						onPressed: {
						}
					}
				}
			}
		}

		GroupBox {
			title: "Status"
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
				rowSpacing: 10
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Time:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: "0"
					Layout.fillWidth: true
				}

				ProgressBar {
					Layout.columnSpan: 2
					Layout.fillWidth: true
				}

				Button {
					text: ZBNT.running ? "Stop" : "Start"
					enabled: ZBNT.connected
					focusPolicy: Qt.NoFocus

					Layout.columnSpan: 2
					Layout.alignment: Qt.AlignRight

					onPressed: {
					}
				}
			}
		}

		Item {
			Layout.fillHeight: true
		}
	}
}
