/*
	zbnt/gui
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

GridLayout {
	id: root
	columns: 3
	rowSpacing: 5
	columnSpacing: 10

	property bool valid: runTimeInput.valid
	property bool updated: bitstreamSelector.currentIndex != ZBNT.bitstreamID || runTimeInput.text != ZBNT.runTime
	property int changePending: 3

	Component.onCompleted: {
		ZBNT.exportResults = Qt.binding(function() { return exportFilesInput.checked })
		ZBNT.openWireshark = Qt.binding(function() { return openWiresharkInput.checked })
	}

	Connections {
		target: ZBNT

		onConnectedChanged: {
			if(ZBNT.connected == ZBNT.Connected)
			{
				ZBNT.getDeviceProperty(0xFF, Messages.PROP_TIMER_LIMIT)
			}
			else
			{
				root.changePending = 3
			}
		}

		onActiveBitstreamChanged: {
			bitstreamSelector.currentIndex = ZBNT.bitstreamNames.indexOf(value)
			ZBNT.bitstreamID = bitstreamSelector.currentIndex
			root.changePending &= ~1
		}

		onPropertyChanged: {
			if(devID == 0xFF && propID == Messages.PROP_TIMER_LIMIT)
			{
				root.changePending &= ~2

				if(success)
				{
					runTimeInput.text = ZBNT.arrayToNum(value, 0, 8)
					ZBNT.runTime = runTimeInput.text
				}
				else
				{
					runTimeInput.text = ZBNT.runTime
				}
			}
		}
	}

	Label {
		text: "Run time:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	UInt64Field {
		id: runTimeInput
		enabled: !ZBNT.running && ZBNT.connected == ZBNT.Connected && !root.changePending

		min: "125000000"
		max: "1152921504606846975"

		Layout.fillWidth: true
	}

	Label {
		text: "cycles"
	}

	Label { }

	ErrorLabel {
		enabled: runTimeInput.enabled
		valid: runTimeInput.valid
		normalText: ZBNT.cyclesToTime(runTimeInput.text)
		errorText: runTimeInput.validator.error
		Layout.columnSpan: 2
	}

	Item {
		Layout.columnSpan: 3
		Layout.minimumHeight: 2
	}

	Label {
		text: "Bitstream:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	ComboBox {
		id: bitstreamSelector
		model: ZBNT.bitstreamNames
		enabled: !ZBNT.running && ZBNT.connected == ZBNT.Connected && !root.changePending

		Layout.fillWidth: true
		Layout.columnSpan: 2

		onCurrentIndexChanged: {
			if(currentIndex == -1)
			{
				displayText = ""
			}
			else
			{
				displayText = ZBNT.bitstreamNames[currentIndex]
			}
		}
	}

	Item {
		Layout.columnSpan: 3
		Layout.minimumHeight: 3
	}

	Label {
		text: "Save results:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	CheckBox {
		id: exportFilesInput
		checked: true
		enabled: !ZBNT.running && ZBNT.connected == ZBNT.Connected
		Layout.fillWidth: true
		Layout.columnSpan: 2

		onToggled: {
			if(!checked) {
				openWiresharkInput.checked = false;
			}
		}
	}

	Label {
		text: "Open Wireshark:"
		font.weight: Font.Bold
		Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
	}

	CheckBox {
		id: openWiresharkInput
		checked: false
		enabled: !ZBNT.running && ZBNT.connected == ZBNT.Connected && exportFilesInput.checked
		Layout.fillWidth: true
		Layout.columnSpan: 2
	}

	RowLayout {
		Layout.columnSpan: 3
		Layout.alignment: Qt.AlignRight

		Button {
			text: "Apply"
			enabled: !ZBNT.running && ZBNT.connected == ZBNT.Connected && root.updated && !root.changePending
			focusPolicy: Qt.NoFocus

			onClicked: {
				if(bitstreamSelector.currentIndex != ZBNT.bitstreamID)
				{
					root.changePending |= 1;
					ZBNT.setActiveBitstream(ZBNT.bitstreamNames[bitstreamSelector.currentIndex])
				}

				root.changePending |= 2;
				ZBNT.setDeviceProperty(0xFF, Messages.PROP_TIMER_LIMIT, ZBNT.arrayFromNum(runTimeInput.text, 8))
			}
		}

		Button {
			text: "Revert"
			enabled: !ZBNT.running && ZBNT.connected == ZBNT.Connected && root.updated && !root.changePending
			focusPolicy: Qt.NoFocus

			onClicked: {
				bitstreamSelector.currentIndex = ZBNT.bitstreamID
				runTimeInput.text = ZBNT.runTime
			}
		}
	}
}
