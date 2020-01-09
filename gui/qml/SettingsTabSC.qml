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

	property var object: undefined
	property var deviceID: 0

	GridLayout {
		columns: 3
		rowSpacing: 5
		columnSpacing: 5
		anchors.fill: parent

		Label {
			text: "Enable: "
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		CheckBox {
			id: enableInput
			enabled: !enableButtons.changePending

			Layout.columnSpan: 2
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
			text: "Sample period: "
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: samplePeriodInput
			enabled: !samplePeriodButtons.changePending

			text: "12500000"
			min: "1"
			max: "4294967295"

			Layout.fillWidth: true

			PropertyButtons {
				id: samplePeriodButtons
				deviceID: root.deviceID
				propertyID: Messages.PROP_SAMPLE_PERIOD

				target: root.object
				targetProperty: "samplePeriod"

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

		Label { }

		ErrorLabel {
			enabled: samplePeriodInput.enabled
			valid: samplePeriodInput.valid
			normalText: ZBNT.cyclesToTime(samplePeriodInput.text)
			errorText: samplePeriodInput.validator.error
			Layout.columnSpan: 2
		}

		Item {
			Layout.fillHeight: true
			Layout.columnSpan: 3
		}

		RowLayout {
			enabled: enableButtons.enabled || samplePeriodButtons.enabled

			Layout.columnSpan: 3
			Layout.alignment: Qt.AlignRight

			Button {
				text: "Apply all"
				focusPolicy: Qt.NoFocus

				onClicked: {
					enableButtons.apply()
					samplePeriodButtons.apply()
				}
			}

			Button {
				text: "Revert all"
				focusPolicy: Qt.NoFocus

				onClicked: {
					enableButtons.undo()
					samplePeriodButtons.undo()
				}
			}
		}
	}
}
