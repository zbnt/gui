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

	property bool ready: runTimeInput.valid
	property string runTime: runTimeInput.text
	property bool exportResults: exportFilesInput.checked

	GridLayout {
		columns: 3
		rowSpacing: 5
		columnSpacing: 10
		anchors.fill: parent

		Label {
			text: "Run time:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		UInt64Field {
			id: runTimeInput

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
			Layout.minimumHeight: 6
		}

		Label {
			text: "Export to CSV files:"
			font.weight: Font.Bold
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		CheckBox {
			id: exportFilesInput
			checked: true
			Layout.fillWidth: true
			Layout.columnSpan: 2
		}

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
