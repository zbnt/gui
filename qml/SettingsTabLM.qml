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
	property bool ready: !enableInput.checked || (frameSizeInput.valid && pingPeriodInput.valid && timeoutInput.valid)

	Component.onCompleted: {
		object.enable = Qt.binding(function() { return enableInput.checked })
		object.frameSize = Qt.binding(function() { return frameSizeInput.text })
		object.period = Qt.binding(function() { return pingPeriodInput.text })
		object.timeout = Qt.binding(function() { return timeoutInput.text })
	}

	GridLayout {
		columns: 3
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
			Layout.columnSpan: 2
		}

		Label {
			text: "Frame size:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: frameSizeInput
			enabled: enableInput.checked

			min: "64"
			max: "1500"

			Layout.fillWidth: true
		}

		Label {
			text: "bytes"
		}

		Label {
			visible: !frameSizeInput.valid
		}

		ErrorLabel {
			enabled: frameSizeInput.enabled
			valid: frameSizeInput.valid
			visible: !valid
			normalText: ""
			errorText: frameSizeInput.validator.error
			Layout.columnSpan: 2
		}

		Item {
			Layout.columnSpan: 3
			Layout.minimumHeight: 6
		}

		Label {
			text: "Inter-ping delay:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: pingPeriodInput
			enabled: enableInput.checked

			text: "12500000"
			min: "1"
			max: "4294967295"

			Layout.fillWidth: true
		}

		Label {
			text: "cycles"
		}

		Label { }

		ErrorLabel {
			enabled: pingPeriodInput.enabled
			valid: pingPeriodInput.valid
			normalText: ZBNT.cyclesToTime(pingPeriodInput.text)
			errorText: pingPeriodInput.validator.error
			Layout.columnSpan: 2
		}

		Item {
			Layout.columnSpan: 3
			Layout.minimumHeight: 6
		}

		Label {
			text: "Timeout:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: timeoutInput
			enabled: enableInput.checked

			text: "125000000"

			min: "1"
			max: "4294967295"

			Layout.fillWidth: true
		}

		Label {
			text: "cycles"
		}

		Label { }

		ErrorLabel {
			enabled: timeoutInput.enabled
			valid: timeoutInput.valid
			normalText: ZBNT.cyclesToTime(timeoutInput.text)
			errorText: timeoutInput.validator.error
			Layout.columnSpan: 2
		}

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
