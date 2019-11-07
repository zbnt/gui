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

GroupBox {
	title: "eth0"
	topPadding: 26
	bottomPadding: 15

	property var object: undefined
	property bool ready: !enableInput.checked || samplePeriodInput.valid

	Component.onCompleted: {
		object.enable = Qt.binding(function() { return enableInput.checked })
		object.samplePeriod = Qt.binding(function() { return samplePeriodInput.text })
	}

	label: Label {
		y: 5
		text: parent.title
		font.weight: Font.Bold
		verticalAlignment: Text.AlignTop
		horizontalAlignment: Text.AlignHCenter
		anchors.horizontalCenter: parent.horizontalCenter
	}

	GridLayout {
		columns: 3
		rowSpacing: 5
		columnSpacing: 5

		anchors.fill: parent
		anchors.topMargin: 5
		anchors.leftMargin: 5
		anchors.rightMargin: 5

		Label {
			text: "Enable: "
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
			text: "Sample period: "
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: samplePeriodInput
			enabled: enableInput.checked
			horizontalAlignment: Qt.AlignHCenter

			min: "1"
			max: "4294967295"

			Layout.fillWidth: true
		}

		Label {
			text: " ms"
		}

		Label {
			visible: !samplePeriodInput.valid
		}

		ErrorLabel {
			visible: !samplePeriodInput.valid
			enabled: samplePeriodInput.enabled
			valid: samplePeriodInput.valid
			normalText: ""
			errorText: samplePeriodInput.validator.error
			Layout.columnSpan: 2
		}
	}
}
