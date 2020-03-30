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
		nameFilters: ["Frame template file (.bin, .hex) (*.bin *.hex)"]

		onAccepted: {
			if(root.object.loadTemplate(fileUrl))
			{
				ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_TEMPLATE, root.object.templateBytes)
				ZBNT.setDeviceProperty(root.deviceID, Messages.PROP_FRAME_TEMPLATE_MASK, root.object.templateMask)
			}
		}
	}

	GridLayout {
		columns: 2
		rowSpacing: 5
		columnSpacing: 10
		anchors.fill: parent

		Label {
			text: "Enable:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		CheckBox {
			id: enableInput
			enabled: !enableButtons.changePending

			Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

			PropertyButtons {
				id: enableButtons
				deviceID: root.deviceID
				propertyID: Messages.PROP_ENABLE

				target: root.object
				targetProperty: "enable"

				input: parent
				inputProperty: "checked"
				inputValid: true

				showRevert: false
				anchors.left: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: 5

				function encodeValue(value) {
					return ZBNT.arrayFromNum(+value, 1)
				}

				function decodeValue(value) {
					return ZBNT.arrayToNum(value, 0, 1)
				}
			}
		}

		Label {
			text: "Frame template:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true

			TextField {
				readOnly: true
				text: root.object.templatePath
				Layout.fillWidth: true
			}

			Button {
				text: "Open"
				focus: false
				focusPolicy: Qt.NoFocus

				onPressed: {
					filePicker.open()
				}
			}
		}

		Label { }

		ErrorLabel {
			valid: root.object.templateLoaded
			normalText: root.object.templateLength + " bytes"
			errorText: "No file selected"
			font.italic: true
		}

		Item {
			Layout.columnSpan: 2
			Layout.minimumHeight: 12
		}

		Label {
			text: "Frame size:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true

			UInt64Field {
				id: sizeInput
				enabled: !sizeButtons.changePending

				min: "60"
				max: "65531"

				Layout.fillWidth: true

				PropertyButtons {
					id: sizeButtons
					deviceID: root.deviceID
					propertyID: Messages.PROP_FRAME_SIZE

					target: root.object
					targetProperty: "frameSize"

					input: parent
					inputProperty: "text"
					inputValid: parent.valid

					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.rightMargin: 10

					function encodeValue(value) {
						return ZBNT.arrayFromNum(value, 2)
					}

					function decodeValue(value) {
						return ZBNT.arrayToNum(value, 0, 2)
					}
				}
			}

			Label {
				text: " bytes"
			}
		}

		Label {
			visible: !sizeInput.valid
		}

		ErrorLabel {
			visible: !sizeInput.valid
			enabled: sizeInput.enabled
			valid: sizeInput.valid
			normalText: ""
			errorText: sizeInput.validator.error
		}

		Label {
			visible: sizeInput.text > 1514 && sizeInput.valid
		}

		ErrorLabel {
			visible: sizeInput.text > 1514 && sizeInput.valid
			enabled: sizeInput.enabled
			valid: false
			wrapMode: Label.WordWrap
			text: "Larger than 1514 bytes, make sure the network has an appropriate MTU"
			Layout.maximumWidth: sizeInput.width
		}

		Label {
			text: "Frame delay:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true

			UInt64Field {
				id: delayInput
				enabled: !delayButtons.changePending

				min: "12"
				max: "4294967295"

				Layout.fillWidth: true

				PropertyButtons {
					id: delayButtons
					deviceID: root.deviceID
					propertyID: Messages.PROP_FRAME_GAP

					target: root.object
					targetProperty: "frameDelay"

					input: parent
					inputProperty: "text"
					inputValid: parent.valid

					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.rightMargin: 10

					function encodeValue(value) {
						return ZBNT.arrayFromNum(value, 4)
					}

					function decodeValue(value) {
						return ZBNT.arrayToNum(value, 0, 4)
					}
				}
			}

			Label {
				text: " cycles"
			}
		}

		Label { }

		ErrorLabel {
			enabled: delayInput.enabled
			valid: delayInput.valid
			normalText: ZBNT.cyclesToTime(delayInput.text)
			errorText: delayInput.validator.error
		}

		Label { }

		Label {
			enabled: delayInput.enabled
			visible: delayInput.valid && sizeInput.valid
			text: "Estimated data rate: " + ZBNT.estimateDataRate(sizeInput.text, delayInput.text)
		}

		Item {
			Layout.columnSpan: 2
			Layout.minimumHeight: 12
		}

		Label {
			text: "Burst mode:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		CheckBox {
			id: burstEnableInput
			enabled: !burstEnableButtons.changePending

			Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

			PropertyButtons {
				id: burstEnableButtons
				deviceID: root.deviceID
				propertyID: Messages.PROP_ENABLE_BURST

				target: root.object
				targetProperty: "burstEnable"

				input: parent
				inputProperty: "checked"
				inputValid: true

				showRevert: false
				anchors.left: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.leftMargin: 5

				function encodeValue(value) {
					return ZBNT.arrayFromNum(+value, 1)
				}

				function decodeValue(value) {
					return ZBNT.arrayToNum(value, 0, 1)
				}
			}
		}

		Label {
			text: "Time on:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true

			UInt64Field {
				id: burstOnTimeInput
				enabled: !burstOnTimeButtons.changePending

				min: "1"
				max: "65535"

				Layout.fillWidth: true

				PropertyButtons {
					id: burstOnTimeButtons
					deviceID: root.deviceID
					propertyID: Messages.PROP_BURST_TIME_ON

					target: root.object
					targetProperty: "burstOnTime"

					input: parent
					inputProperty: "text"
					inputValid: parent.valid

					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.rightMargin: 10

					function encodeValue(value) {
						return ZBNT.arrayFromNum(value, 2)
					}

					function decodeValue(value) {
						return ZBNT.arrayToNum(value, 0, 2)
					}
				}
			}

			Label {
				text: " ms"
			}
		}

		Label {
			visible: !burstOnTimeInput.valid
			Layout.minimumHeight: 24
		}

		ErrorLabel {
			enabled: burstOnTimeInput.enabled
			valid: burstOnTimeInput.valid
			visible: !valid
			normalText: ""
			errorText: burstOnTimeInput.validator.error
			Layout.alignment: Qt.AlignTop | Qt.AlignLeft
		}

		Label {
			text: "Time off:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true

			UInt64Field {
				id: burstOffTimeInput
				enabled: !burstOffTimeButtons.changePending

				min: "1"
				max: "65535"

				Layout.fillWidth: true

				PropertyButtons {
					id: burstOffTimeButtons
					deviceID: root.deviceID
					propertyID: Messages.PROP_BURST_TIME_OFF

					target: root.object
					targetProperty: "burstOffTime"

					input: parent
					inputProperty: "text"
					inputValid: parent.valid

					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.rightMargin: 10

					function encodeValue(value) {
						return ZBNT.arrayFromNum(value, 2)
					}

					function decodeValue(value) {
						return ZBNT.arrayToNum(value, 0, 2)
					}
				}
			}

			Label {
				text: " ms"
			}
		}

		Label {
			visible: !burstOffTimeInput.valid
		}

		ErrorLabel {
			enabled: burstOffTimeInput.enabled
			valid: burstOffTimeInput.valid
			visible: !valid
			normalText: ""
			errorText: burstOffTimeInput.validator.error
		}

		Item {
			Layout.columnSpan: 2
			Layout.minimumHeight: 12
		}

		Label {
			text: "LFSR Seed:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: seedInput
			enabled: !seedButtons.changePending

			min: "0"
			max: "255"

			Layout.fillWidth: true

			PropertyButtons {
				id: seedButtons
				deviceID: root.deviceID
				propertyID: Messages.PROP_LFSR_SEED

				target: root.object
				targetProperty: "lfsrSeed"

				input: parent
				inputProperty: "text"
				inputValid: parent.valid

				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.rightMargin: 10

				function encodeValue(value) {
					return ZBNT.arrayFromNum(value, 1)
				}

				function decodeValue(value) {
					return ZBNT.arrayToNum(value, 0, 1)
				}
			}
		}

		Label {
			visible: !seedInput.valid && seedInput.text.length
		}

		ErrorLabel {
			enabled: seedInput.enabled
			valid: seedInput.valid || !seedInput.text.length
			visible: !valid
			normalText: ""
			errorText: seedInput.validator.error
		}

		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 2
		}

		RowLayout {
			Layout.columnSpan: 2
			Layout.alignment: Qt.AlignRight

			Button {
				text: "Apply all"
				focusPolicy: Qt.NoFocus

				enabled: enableButtons.canApply || sizeButtons.canApply || delayButtons.canApply || burstEnableButtons.canApply || burstOnTimeButtons.canApply || burstOffTimeButtons.canApply || seedButtons.canApply

				onClicked: {
					enableButtons.apply()
					sizeButtons.apply()
					delayButtons.apply()
					burstEnableButtons.apply()
					burstOnTimeButtons.apply()
					burstOffTimeButtons.apply()
					seedButtons.apply()
				}
			}

			Button {
				text: "Revert all"
				focusPolicy: Qt.NoFocus

				enabled: enableButtons.canRevert || sizeButtons.canRevert || delayButtons.canRevert || burstEnableButtons.canRevert || burstOnTimeButtons.canRevert || burstOffTimeButtons.canRevert || seedButtons.canRevert

				onClicked: {
					enableButtons.undo()
					sizeButtons.undo()
					delayButtons.undo()
					burstEnableButtons.undo()
					burstOnTimeButtons.undo()
					burstOffTimeButtons.undo()
					seedButtons.undo()
				}
			}
		}
	}
}
