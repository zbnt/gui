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

Item {
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

		TextField {
			Layout.fillWidth: true
		}

		Label {
			text: "cycles"
		}

		Label { }

		Label {
			text: "8 [ns]"
			font.italic: true
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
			Layout.fillWidth: true
			Layout.columnSpan: 2
		}

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
