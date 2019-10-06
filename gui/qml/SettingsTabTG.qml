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
	property bool ready: !ZBNT.streamMode && (!enableInput.checked || sizeInput.valid && delayInput.valid && (!burstEnableInput.checked || (burstOnTimeInput.valid && burstOffTimeInput.valid)) && seedInput.valid) || ZBNT.streamMode && seedInput.valid

	property bool templateLoaded: object.templateLoaded
	property string templatePath: object.templatePath
	property int templateLength: object.templateLength

	Component.onCompleted: {
		object.enable = Qt.binding(function() { return enableInput.checked })

		object.frameSize = Qt.binding(function() { return sizeInput.text })
		object.frameDelay = Qt.binding(function() { return delayInput.text })

		object.burstEnable = Qt.binding(function() { return burstEnableInput.checked })
		object.burstOnTime = Qt.binding(function() { return burstOnTimeInput.text })
		object.burstOffTime = Qt.binding(function() { return burstOffTimeInput.text })

		object.lfsrSeed = Qt.binding(function() { return seedInput.text })
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
			root.object.loadTemplate(fileUrl);
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
			checked: false
			enabled: !ZBNT.streamMode
			Layout.fillWidth: true
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
				text: root.templatePath
				enabled: enableInput.checked || ZBNT.streamMode
				color: !enabled ? DisabledLabel.color : (root.templateLength <= 1500 ? DefaultLabel.color : "#e53935")
				Layout.fillWidth: true
			}

			Button {
				text: "Open"
				enabled: enableInput.checked || ZBNT.streamMode
				focus: false
				focusPolicy: Qt.NoFocus

				onPressed: {
					filePicker.open()
				}
			}
		}

		Label { }

		ErrorLabel {
			valid: root.templateLoaded && root.templateLength <= 1500
			enabled: enableInput.checked || ZBNT.streamMode
			normalText: root.templateLength + " bytes"
			errorText: !root.templateLoaded ? "No file selected" : (normalText + " - Must be at most 1500 bytes")
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
				enabled: enableInput.checked && !ZBNT.streamMode
				horizontalAlignment: Qt.AlignHCenter

				min: "60"
				max: "1500"

				Layout.fillWidth: true
			}

			Label {
				text: " bytes"
			}
		}

		Label {
			visible: !sizeInput.valid
			Layout.minimumHeight: 14
		}

		ErrorLabel {
			visible: !sizeInput.valid
			enabled: sizeInput.enabled
			valid: sizeInput.valid
			normalText: ""
			errorText: sizeInput.validator.error
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
				enabled: enableInput.checked && !ZBNT.streamMode
				horizontalAlignment: Qt.AlignHCenter

				min: "12"
				max: "4294967295"

				Layout.fillWidth: true
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
			visible: delayInput.valid && sizeInput.valid // ZBNT.bitsToHumanReadable
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
			checked: false
			enabled: enableInput.checked && !ZBNT.streamMode
			Layout.fillWidth: true
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
				enabled: burstEnableInput.checked && enableInput.checked && !ZBNT.streamMode
				horizontalAlignment: Qt.AlignHCenter

				min: "1"
				max: "65535"

				Layout.fillWidth: true
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
				enabled: burstEnableInput.checked && enableInput.checked && !ZBNT.streamMode
				horizontalAlignment: Qt.AlignHCenter

				min: "1"
				max: "65535"

				Layout.fillWidth: true
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
			enabled: enableInput.checked || ZBNT.streamMode
			horizontalAlignment: Qt.AlignHCenter

			min: "0"
			max: "255"

			Layout.fillWidth: true
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

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
