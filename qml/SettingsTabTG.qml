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
	property bool ready: !enableInput.checked || (headersLoaded && delayMethodSelector.valid)

	property bool headersLoaded: object.headersLoaded
	property string headersPath: object.headersPath
	property int headersLength: object.headersLength

	Component.onCompleted: {
		object.enable = Qt.binding(function() { return enableInput.checked })
		object.paddingMethod = Qt.binding(function() { return paddingMethodSelector.currentIndex })
		object.paddingConstant = Qt.binding(function() { return paddingConstantInput.value })
		object.paddingRangeBottom = Qt.binding(function() { return paddingRangeBottomInput.value })
		object.paddingRangeTop = Qt.binding(function() { return paddingRangeTopInput.value })
		object.paddingAverage = Qt.binding(function() { return paddingAverageInput.value })
		object.delayMethod = Qt.binding(function() { return delayMethodSelector.currentIndex })
		object.delayConstant = Qt.binding(function() { return delayConstantInput.text })
		object.delayRangeBottom = Qt.binding(function() { return delayRangeBottomInput.text })
		object.delayRangeTop = Qt.binding(function() { return delayRangeTopInput.text })
		object.delayAverage = Qt.binding(function() { return delayAverageInput.text })
	}

	FileDialog {
		id: filePicker
		title: "Pick a file"
		folder: ""

		selectExisting: true
		selectMultiple: false
		selectFolder: false
		nameFilters: ["Headers file (.bin, .hex) (*.bin *.hex)"]

		onAccepted: {
			root.object.loadHeaders(fileUrl);

			if(root.headersLength >= 1500)
			{
				paddingMethodSelector.currentIndex = 0;
				paddingConstantInput.value = 0;
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
			checked: true
			Layout.fillWidth: true
		}

		Label {
			text: "Frame headers:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true

			TextField {
				readOnly: true
				text: root.headersPath
				enabled: enableInput.checked
				color: !enabled ? DisabledLabel.color : ((root.headersLength >= 14 && root.headersLength <= 1500) ? DefaultLabel.color : "#e53935")
				Layout.fillWidth: true
			}

			Button {
				text: "Open"
				enabled: enableInput.checked
				focus: false
				focusPolicy: Qt.NoFocus

				onPressed: {
					filePicker.open()
				}
			}
		}

		Label { }

		ErrorLabel {
			valid: root.headersLoaded && root.headersLength >= 14 && root.headersLength <= 1500
			enabled: enableInput.checked
			normalText: root.headersLength + " bytes"
			errorText: !root.headersLoaded ? "No file selected" : (normalText + " - Must be at " + (root.headersLength >= 14 ? "most 1500" : "least 64") + " bytes")
			font.italic: true
		}

		Item {
			Layout.columnSpan: 2
			Layout.minimumHeight: 12
		}

		Label {
			text: "Frame padding:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		ComboBox {
			id: paddingMethodSelector
			enabled: root.headersLoaded && enableInput.checked && root.headersLength >= 14 && root.headersLength < 1500

			Layout.fillWidth: true

			model: [
				"Constant",
				"Random - Uniform",
				"Random - Poisson"
			]
		}

		Label {
			text: "Amount:"
			font.weight: Font.Bold
			visible: paddingMethodSelector.currentIndex == 0
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: paddingMethodSelector.currentIndex == 0

			SpinBox {
				id: paddingConstantInput

				from: 0
				to: 1500 - root.headersLength

				editable: true
				enabled: root.headersLoaded && enableInput.checked && root.headersLength >= 14 && root.headersLength < 1500

				Layout.fillWidth: true
			}

			Label {
				text: " bytes"
			}
		}

		Label {
			text: "Range:"
			font.weight: Font.Bold
			visible: paddingMethodSelector.currentIndex == 1
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: paddingMethodSelector.currentIndex == 1

			SpinBox {
				id: paddingRangeBottomInput

				from: 0
				to: 1500 - root.headersLength

				editable: true
				enabled: root.headersLoaded && enableInput.checked

				onValueChanged: {
					if(value > paddingRangeTopInput.value)
						value = paddingRangeTopInput.value;
				}

				Layout.fillWidth: true
			}

			Label {
				text: " - "
			}

			SpinBox {
				id: paddingRangeTopInput

				from: 0
				to: 1500 - root.headersLength

				editable: true
				enabled: root.headersLoaded && enableInput.checked

				onValueChanged: {
					if(paddingRangeBottomInput.value > value)
						paddingRangeBottomInput.value = value;
				}

				Layout.fillWidth: true
			}

			Label {
				text: " bytes"
			}
		}

		Label {
			text: "Average:"
			font.weight: Font.Bold
			visible: paddingMethodSelector.currentIndex == 2
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: paddingMethodSelector.currentIndex == 2

			SpinBox {
				id: paddingAverageInput

				from: 0
				to: 1500 - root.headersLength

				editable: true
				enabled: root.headersLoaded && enableInput.checked

				Layout.fillWidth: true
			}

			Label {
				text: " bytes/frame"
			}
		}

		Item {
			Layout.columnSpan: 2
			Layout.minimumHeight: 12
		}

		Label {
			text: "Inter-frame delay:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		ComboBox {
			id: delayMethodSelector
			enabled: root.headersLoaded && enableInput.checked
			Layout.fillWidth: true

			property bool valid: [delayConstantInput.valid, delayRangeBottomInput.valid && delayRangeTopInput.valid && delayRangeValidator.valid, delayAverageInput.valid][currentIndex]

			model: [
				"Constant",
				"Random - Uniform",
				"Random - Poisson"
			]
		}

		Label {
			text: "Amount:"
			font.weight: Font.Bold
			visible: delayMethodSelector.currentIndex == 0
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: delayMethodSelector.currentIndex == 0

			UInt64Field {
				id: delayConstantInput
				enabled: root.headersLoaded && enableInput.checked
				horizontalAlignment: Qt.AlignHCenter

				min: "0"
				max: "4294967295"

				Layout.fillWidth: true
			}

			Label {
				text: " cycles"
			}
		}

		Label {
			visible: delayMethodSelector.currentIndex == 0
		}

		ErrorLabel {
			visible: delayMethodSelector.currentIndex == 0
			enabled: delayConstantInput.enabled
			valid: delayConstantInput.valid
			normalText: ZBNT.cyclesToTime(delayConstantInput.text)
			errorText: delayConstantInput.validator.error
		}

		Label {
			text: "Range:"
			font.weight: Font.Bold
			visible: delayMethodSelector.currentIndex == 1
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: delayMethodSelector.currentIndex == 1

			UInt64Field {
				id: delayRangeBottomInput
				enabled: root.headersLoaded && enableInput.checked
				horizontalAlignment: Qt.AlignHCenter

				text: "0"
				min: "0"
				max: "4294967295"

				Layout.fillWidth: true
			}

			Label {
				text: " - "
			}

			UInt64Field {
				id: delayRangeTopInput
				enabled: root.headersLoaded && enableInput.checked
				horizontalAlignment: Qt.AlignHCenter

				text: "1"
				min: "1"
				max: "4294967295"

				Layout.fillWidth: true

				UInt64RangeValidator {
					id: delayRangeValidator
					top: delayRangeTopInput.text
					bottom: delayRangeBottomInput.text
				}
			}

			Label {
				text: " cycles"
			}
		}

		Label {
			visible: delayMethodSelector.currentIndex == 1 && delayRangeBottomInput.valid && delayRangeTopInput.valid && delayRangeValidator.valid
		}

		Label {
			visible: delayMethodSelector.currentIndex == 1 && delayRangeBottomInput.valid && delayRangeTopInput.valid && delayRangeValidator.valid
			text: ZBNT.cyclesToTime(delayRangeBottomInput.text) + " to " + ZBNT.cyclesToTime(delayRangeTopInput.text)
		}

		Label {
			visible: delayMethodSelector.currentIndex == 1 && !delayRangeBottomInput.valid
		}

		ErrorLabel {
			visible: delayMethodSelector.currentIndex == 1 && !delayRangeBottomInput.valid
			enabled: delayRangeBottomInput.enabled
			valid: false
			normalText: ""
			errorText: delayRangeBottomInput.validator.error.replace("M", "Minimum m")
		}

		Label {
			visible: delayMethodSelector.currentIndex == 1 && !delayRangeTopInput.valid
		}

		ErrorLabel {
			visible: delayMethodSelector.currentIndex == 1 && !delayRangeTopInput.valid
			enabled: delayRangeTopInput.enabled
			valid: false
			normalText: ""
			errorText: delayRangeTopInput.validator.error.replace("M", "Maximum m")
		}

		Label {
			visible: delayMethodSelector.currentIndex == 1 && !delayRangeValidator.valid
		}

		ErrorLabel {
			visible: delayMethodSelector.currentIndex == 1 && !delayRangeValidator.valid
			enabled: delayRangeTopInput.enabled
			valid: false
			normalText: ""
			errorText: "Invalid range"
		}

		Label {
			text: "Average:"
			font.weight: Font.Bold
			visible: delayMethodSelector.currentIndex == 2
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: delayMethodSelector.currentIndex == 2

			UInt64Field {
				id: delayAverageInput
				enabled: root.headersLoaded && enableInput.checked
				horizontalAlignment: Qt.AlignHCenter

				min: "0"
				max: "4294967295"

				Layout.fillWidth: true
			}

			Label {
				text: " cycles/frame"
			}
		}

		Label {
			visible: delayMethodSelector.currentIndex == 2
		}

		ErrorLabel {
			visible: delayMethodSelector.currentIndex == 2
			enabled: delayAverageInput.enabled
			valid: delayAverageInput.valid
			normalText: ZBNT.cyclesToTime(delayAverageInput.text) + " per frame"
			errorText: delayAverageInput.validator.error
		}

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
