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
	property int scriptID: 0
	property int changePending: 0

	function updateScript(id, path) {
		var scriptBytes = root.object.loadScript(id, path)

		if(scriptBytes.byteLength)
		{
			root.changePending |= 4
			ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_SCRIPT, scriptBytes)
			ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_SCRIPT_NAME, ZBNT.arrayConcat(ZBNT.arrayFromNum(id, 4), ZBNT.arrayFromStr(root.object.scriptName[id])))
			ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_ENABLE_SCRIPT, ZBNT.arrayFromNum(root.object.scriptsEnabled, 4))
		}
	}

	function clearScript(id) {
		root.changePending |= 4
		root.object.removeScript(id)
		ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_SCRIPT, ZBNT.arrayFromNum(id, 4))
		ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_SCRIPT_NAME, ZBNT.arrayFromNum(id, 4))
		ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_ENABLE_SCRIPT, ZBNT.arrayFromNum(root.object.scriptsEnabled, 4))
	}

	Component.onCompleted: {
		root.changePending = 7

		for(var i = 0; i < 2*root.object.numScripts; i++)
		{
			ZBNT.getDevicePropertyWithArgs(root.deviceID, Messages.PROP_FRAME_SCRIPT_NAME, ZBNT.arrayFromNum(i, 4));
		}

		ZBNT.getDeviceProperty(root.deviceID, Messages.PROP_ENABLE)
		ZBNT.getDeviceProperty(root.deviceID, Messages.PROP_ENABLE_LOG)
		ZBNT.getDeviceProperty(root.deviceID, Messages.PROP_ENABLE_SCRIPT)
	}

	Connections {
		target: ZBNT

		onPropertyChanged: {
			if(devID == root.deviceID)
			{
				if(propID == Messages.PROP_ENABLE)
				{
					if(success)
					{
						enableInput.checked = ZBNT.arrayToNum(value, 0, 1)
						root.object.enable = enableInput.checked
					}
					else
					{
						enableInput.checked = root.object.enable
					}

					root.changePending &= ~1
				}
				else if(propID == Messages.PROP_ENABLE_LOG)
				{
					if(success)
					{
						logEnableInput.checked = ZBNT.arrayToNum(value, 0, 1)
						root.object.logEnable = logEnableInput.checked
					}
					else
					{
						logEnableInput.checked = root.object.logEnable
					}

					root.changePending &= ~2
				}
				else if(propID == Messages.PROP_ENABLE_SCRIPT)
				{
					if(success)
					{
						root.object.scriptsEnabled = ZBNT.arrayToNum(value, 0, 4)
					}

					root.changePending &= ~4
				}
				else if(propID == Messages.PROP_FRAME_SCRIPT_NAME)
				{
					if(success)
					{
						var index = ZBNT.arrayToNum(value, 0, 4)

						if(index < 2*root.object.numScripts)
						{
							root.object.scriptName[index] = ZBNT.arrayToStr(value, 4, -1)
						}
					}
				}
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
		nameFilters: ["ZBNT Script (.zbscr) (*.zbscr)"]

		onAccepted: {
			root.updateScript(root.scriptID, fileUrl)
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		GroupBox {
			title: "Settings"
			topPadding: 26
			bottomPadding: 15
			Layout.columnSpan: 2

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
				rowSpacing: 5
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Item {
					Layout.rowSpan: 5
					Layout.fillWidth: true
				}

				Label {
					text: "Enable: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				CheckBox {
					id: enableInput
					enabled: !root.changePending

					Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

					onCheckedChanged: {
						if(enabled)
						{
							root.changePending |= 1
							ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_ENABLE, ZBNT.arrayFromNum(+checked, 1))
						}
					}
				}

				Item {
					Layout.rowSpan: 5
					Layout.fillWidth: true
				}

				Label {
					text: "Enable log: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				CheckBox {
					id: logEnableInput
					enabled: !root.changePending

					Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

					onCheckedChanged: {
						if(enabled)
						{
							root.changePending |= 2
							ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_ENABLE_LOG, ZBNT.arrayFromNum(+checked, 1))
						}
					}
				}

				Label {
					text: "Capabilities: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				RowLayout {
					spacing: 10

					CapabilityDisplay {
						text: "Comparator"
						available: !!(root.object.featureBits & 1)
					}

					CapabilityDisplay {
						text: "Editor"
						available: !!(root.object.featureBits & 2)
					}

					CapabilityDisplay {
						text: "Checksum"
						available: !!(root.object.featureBits & 4)
					}

					CapabilityDisplay {
						text: "FPU"
						available: !!(root.object.featureBits & 8)
					}
				}

				Label {
					text: "Max script size: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				Label {
					text: root.object.maxScriptSize + " words per unit"
				}

				Label {
					text: "FIFO sizes: "
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				Label {
					text: root.object.txFifoSize + " bytes for transmission, " + root.object.extrFifoSize + " bytes for extraction"
				}
			}
		}

		GroupBox {
			title: "Scripts (eth" + root.object.portA + " → eth" + root.object.portB + ")"
			topPadding: 26
			bottomPadding: 15
			Layout.columnSpan: 2

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
				columns: 5
				rowSpacing: 10
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Repeater {
					model: root.object.numScripts

					Label {
						text: "Script " + (index + 1) + ": "
						font.weight: Font.Bold
						Layout.row: index
						Layout.column: 0
						Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
					}
				}

				Repeater {
					model: root.object.numScripts

					TextField {
						readOnly: true
						text: " " + root.object.scriptName[index]
						Layout.row: index
						Layout.column: 1
						Layout.fillWidth: true
					}
				}

				Repeater {
					model: root.object.numScripts

					Button {
						icon.name: "folder-new"
						focus: false
						enabled: !ZBNT.running && !root.changePending
						focusPolicy: Qt.NoFocus

						Layout.row: index
						Layout.column: 2

						onPressed: {
							root.scriptID = index
							filePicker.open()
						}
					}
				}

				Repeater {
					model: root.object.numScripts

					Button {
						icon.name: "view-refresh"
						focus: false
						enabled: !ZBNT.running && !root.changePending && root.object.scriptPath[index].toString().length
						focusPolicy: Qt.NoFocus

						Layout.row: index
						Layout.column: 3

						onPressed: {
							root.updateScript(index, root.object.scriptPath[index])
						}
					}
				}

				Repeater {
					model: root.object.numScripts

					Button {
						icon.name: "delete"
						focus: false
						enabled: !ZBNT.running && !root.changePending
						focusPolicy: Qt.NoFocus

						Layout.row: index
						Layout.column: 4

						onPressed: {
							root.clearScript(index)
						}
					}
				}
			}
		}

		GroupBox {
			title: "Scripts (eth" + root.object.portB + " → eth" + root.object.portA + ")"
			topPadding: 26
			bottomPadding: 15
			Layout.columnSpan: 2

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
				columns: 5
				rowSpacing: 10
				columnSpacing: 5

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Repeater {
					model: root.object.numScripts

					Label {
						text: "Script " + (index + 1) + ": "
						font.weight: Font.Bold
						Layout.row: index
						Layout.column: 0
						Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
					}
				}

				Repeater {
					model: root.object.numScripts

					TextField {
						readOnly: true
						text: " " + root.object.scriptName[index + root.object.numScripts]
						Layout.row: index
						Layout.column: 1
						Layout.fillWidth: true
					}
				}

				Repeater {
					model: root.object.numScripts

					Button {
						icon.name: "folder-new"
						focus: false
						enabled: !ZBNT.running && !root.changePending
						focusPolicy: Qt.NoFocus

						Layout.row: index
						Layout.column: 2

						onPressed: {
							root.scriptID = index + root.object.numScripts
							filePicker.open()
						}
					}
				}

				Repeater {
					model: root.object.numScripts

					Button {
						icon.name: "view-refresh"
						focus: false
						enabled: !ZBNT.running && !root.changePending && root.object.scriptPath[index + root.object.numScripts].toString().length
						focusPolicy: Qt.NoFocus

						Layout.row: index
						Layout.column: 3

						onPressed: {
							root.updateScript(index + root.object.numScripts, root.object.scriptPath[index + root.object.numScripts])
						}
					}
				}

				Repeater {
					model: root.object.numScripts

					Button {
						icon.name: "delete"
						focus: false
						enabled: !ZBNT.running && !root.changePending
						focusPolicy: Qt.NoFocus

						Layout.row: index
						Layout.column: 4

						onPressed: {
							root.clearScript(index + root.object.numScripts)
						}
					}
				}
			}
		}

		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}
	}
}
