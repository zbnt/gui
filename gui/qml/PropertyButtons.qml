/*
	zbnt_gui
	Copyright (C) 2020 Oscar R.

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

RowLayout {
	id: root
	spacing: 8

	property int deviceID: 0
	property int propertyID: 0

	property var target: undefined
	property string targetProperty: ""

	property var input: undefined
	property var inputProperty: ""
	property var inputValid: true

	property bool showRevert: true
	property int fontSize: 16

	property bool changePending: true
	property bool valueUpdated: input[inputProperty] != target[targetProperty]

	function encodeValue(value) {
		return ""
	}

	function decodeValue(value) {
		return ""
	}

	function apply() {
		if(!changePending && valueUpdated && inputValid)
		{
			root.changePending = true
			ZBNT.setDeviceProperty(deviceID, propertyID, encodeValue(input[inputProperty]))
		}
	}

	function undo() {
		if(!changePending && valueUpdated)
		{
			input[inputProperty] = target[targetProperty]
		}
	}

	Component.onCompleted: {
		root.changePending = true
		ZBNT.getDeviceProperty(root.deviceID, root.propertyID)
	}

	Connections {
		target: ZBNT

		onPropertyChanged: {
			if(devID == root.deviceID && propID == root.propertyID)
			{
				root.changePending = false

				if(success)
				{
					root.target[root.targetProperty] = decodeValue(value)
				}

				root.input[root.inputProperty] = root.target[root.targetProperty]
			}
		}
	}

	Label {
		text: "\uE876"
		enabled: !changePending && valueUpdated && inputValid
		font.family: "Material Icons"
		font.pointSize: root.fontSize
		color: mouseAreaA.pressed ? DefaultLabel.color : (mouseAreaA.containsMouse ? "#81c784" : "#43a047")

		ToolTip.text: "Apply"
		ToolTip.visible: mouseAreaA.containsMouse

		MouseArea {
			id: mouseAreaA
			anchors.fill: parent
			hoverEnabled: true

			onClicked: root.apply()
		}
	}

	Label {
		text: "\uE166"
		enabled: !changePending && valueUpdated
		visible: root.showRevert
		font.family: "Material Icons"
		font.pointSize: root.fontSize
		color: mouseAreaB.pressed ? DefaultLabel.color : (mouseAreaB.containsMouse ? "#e57373" : "#e53935")

		ToolTip.text: "Revert"
		ToolTip.visible: mouseAreaB.containsMouse

		MouseArea {
			id: mouseAreaB
			anchors.fill: parent
			hoverEnabled: true

			onClicked: root.undo()
		}
	}

	Layout.columnSpan: 2
	Layout.alignment: Qt.AlignRight
}
