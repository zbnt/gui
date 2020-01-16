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
	property var columnWidths: [150, 130, width - 280]

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
				font.weight: Font.Bold
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			GridLayout {
				columns: 4
				rowSpacing: 4
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "eth" + root.object.portA + " → eth" + root.object.portB
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignHCenter
					Layout.minimumWidth: parent.width / 2
					Layout.columnSpan: 2
				}

				Label {
					text: "eth" + root.object.portB + " → eth" + root.object.portA
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignHCenter
					Layout.minimumWidth: parent.width / 2
					Layout.columnSpan: 2
				}

				Label {
					text: "Pattern 1:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[0]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 1:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[4]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 2:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[1]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 2:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[5]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 3:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[2]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 3:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[6]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 4:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[3]
					Layout.fillWidth: true
				}

				Label {
					text: "Pattern 4:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 4
				}

				Label {
					text: root.object.detectionCounters[7]
					Layout.fillWidth: true
				}
			}
		}

		GroupBox {
			title: "Last 1000 detections (eth" + root.object.portA + " → eth" + root.object.portB + ")"
			topPadding: 26
			bottomPadding: 15

			label: Label {
				y: 5
				text: parent.title
				font.weight: Font.Bold
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true
			Layout.fillHeight: true

			GridLayout {
				columns: 3
				rowSpacing: 5
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Time [ns]"
					font.weight: Font.Bold
					Layout.minimumWidth: root.columnWidths[0]
				}

				Label {
					text: "Patterns"
					font.weight: Font.Bold
					Layout.minimumWidth: root.columnWidths[1]
				}

				Label {
					text: "Extracted data"
					font.weight: Font.Bold
					Layout.fillWidth: true
				}

				TableView {
					id: tableA

					clip: true
					rowSpacing: 1
					columnSpacing: 10
					flickableDirection: Flickable.VerticalFlick
					model: root.object.detectionListA

					ScrollBar.vertical: ScrollBar { }
					Layout.columnSpan: 3
					Layout.fillHeight: true
					Layout.fillWidth: true

					columnWidthProvider: function (column) { return root.columnWidths[column] }

					delegate: Label {
						text: display
					}
				}
			}
		}

		GroupBox {
			title: "Last 1000 detections (eth" + root.object.portB + " → eth" + root.object.portA + ")"
			topPadding: 26
			bottomPadding: 15

			label: Label {
				y: 5
				text: parent.title
				font.weight: Font.Bold
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true
			Layout.fillHeight: true

			GridLayout {
				columns: 3
				rowSpacing: 5
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Time [ns]"
					font.weight: Font.Bold
					Layout.minimumWidth: root.columnWidths[0]
				}

				Label {
					text: "Patterns"
					font.weight: Font.Bold
					Layout.minimumWidth: root.columnWidths[1]
				}

				Label {
					text: "Extracted data"
					font.weight: Font.Bold
					Layout.fillWidth: true
				}

				TableView {
					id: tableB

					clip: true
					rowSpacing: 1
					columnSpacing: 10
					flickableDirection: Flickable.VerticalFlick
					model: root.object.detectionListB

					ScrollBar.vertical: ScrollBar { }
					Layout.columnSpan: 3
					Layout.fillHeight: true
					Layout.fillWidth: true

					columnWidthProvider: function (column) { return root.columnWidths[column] }

					delegate: Label {
						text: display
					}
				}
			}
		}
	}
}
