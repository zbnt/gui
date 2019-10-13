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
import QtQuick.Dialogs 1.3

import zbnt 1.0

Item {
	id: root

	property var object: undefined
	property int patternID: 0

	FileDialog {
		id: filePicker
		title: "Pick a file"
		folder: ""

		selectExisting: true
		selectMultiple: false
		selectFolder: false
		nameFilters: ["Filter pattern (.hex) (*.hex)"]

		onAccepted: {
			root.object.loadPattern(root.patternID, fileUrl);
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		GroupBox {
			title: "Patterns (eth2 → eth3)"
			topPadding: 26
			bottomPadding: 15
			Layout.columnSpan: 2

			label: Label {
				y: 5
				text: parent.title
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			GridLayout {
				columns: 4
				rowSpacing: 10
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Pattern 1: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[0]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 0
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(0)
					}
				}

				Label {
					text: "Pattern 2: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[1]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 1
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(1)
					}
				}

				Label {
					text: "Pattern 3: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[2]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 2
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(2)
					}
				}

				Label {
					text: "Pattern 4: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[3]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 3
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(3)
					}
				}
			}
		}

		GroupBox {
			title: "Patterns (eth3 → eth2)"
			topPadding: 26
			bottomPadding: 15
			Layout.columnSpan: 2

			label: Label {
				y: 5
				text: parent.title
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			GridLayout {
				columns: 4
				rowSpacing: 10
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Pattern 1: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[4]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 4
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(4)
					}
				}

				Label {
					text: "Pattern 2: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[5]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 5
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(5)
					}
				}

				Label {
					text: "Pattern 3: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[6]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 6
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(6)
					}
				}

				Label {
					text: "Pattern 4: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				TextField {
					readOnly: true
					text: root.object.patternPath[7]
					Layout.fillWidth: true
				}

				Button {
					icon.name: "folder-new"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 7
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(7)
					}
				}
			}
		}

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
