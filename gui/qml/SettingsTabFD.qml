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
	property var deviceID: 0
	property int patternID: 0
	property int changePending: 0

	Component.onCompleted: {
		root.changePending = 1
		ZBNT.getDeviceProperty(root.deviceID, Messages.PROP_ENABLE_CSUM_FIX)
	}

	Connections {
		target: ZBNT

		onPropertyChanged: {
			if(devID == root.deviceID && propID == Messages.PROP_ENABLE_CSUM_FIX)
			{
				if(success)
				{
					fixChecksumsInput.checked = ZBNT.arrayToNum(value, 0, 1)
					root.object.fixChecksums = fixChecksumsInput.checked
				}
				else
				{
					fixChecksumsInput.checked = root.object.fixChecksums
				}

				root.changePending = 0
			}
		}
	}

	Connections {
		target: root.object

		onError: {
			errorDialog.text = msg;
			errorDialog.open();
		}
	}

	MessageDialog {
		id: errorDialog
		title: "Error"
		text: ""
		icon: StandardIcon.Critical
		standardButtons: StandardButton.Ok
	}

	FileDialog {
		id: filePicker
		title: "Pick a file"
		folder: ""

		selectExisting: true
		selectMultiple: false
		selectFolder: false
		nameFilters: ["Filter pattern (.hex) (*.hex)"]

		onAccepted: {
			if(root.object.loadPattern(root.patternID, fileUrl))
			{
				ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[root.patternID])
				ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[root.patternID])
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		GroupBox {
			title: "Patterns (eth" + root.object.portA + " → eth" + root.object.portB + ")"
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 0
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(0)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[0])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[0])
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 1
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(1)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[1])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[1])
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 2
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(2)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[2])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[2])
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 3
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(3)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[3])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[3])
					}
				}
			}
		}

		GroupBox {
			title: "Patterns (eth" + root.object.portB + " → eth" + root.object.portA + ")"
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 4
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(4)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[4])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[4])
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 5
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(5)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[5])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[5])
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 6
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(6)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[6])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[6])
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
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.patternID = 7
						filePicker.open()
					}
				}

				Button {
					icon.name: "delete"
					focus: false
					enabled: !ZBNT.running
					focusPolicy: Qt.NoFocus

					onPressed: {
						root.object.removePattern(7)
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN, root.object.patternBytes[7])
						ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_PATTERN_FLAGS, root.object.patternFlags[7])
					}
				}
			}
		}

		GroupBox {
			title: "Other settings"
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
				columns: 3
				rowSpacing: 10
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Item {
					Layout.fillWidth: true
				}

				CheckBox {
					id: fixChecksumsInput
					enabled: !root.changePending && !ZBNT.running
					checked: true
					text: "Fix TCP/UDP/ICMP/ICMPv6 checksums if needed"

					onCheckedChanged: {
						if(enabled)
						{
							root.changePending = 1
							ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_ENABLE_CSUM_FIX, ZBNT.arrayFromNum(+checked, 1))
						}
					}
				}

				Item {
					Layout.fillWidth: true
				}
			}
		}

		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}
	}
}
