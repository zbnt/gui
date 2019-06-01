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
				Layout.fillWidth: true
			}

			Button {
				text: "Open"
			}
		}

		Label { }

		Label {
			text: "No file selected"
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
				from: 0
				to: 1500
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
				from: 0
				to: 1500
				Layout.fillWidth: true
			}

			Label {
				text: " - "
			}

			SpinBox {
				from: 0
				to: 1500
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
				from: 0
				to: 1500
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
			visible: delayMethodSelector.currentIndex == 0
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
		}

		RowLayout {
			Layout.fillWidth: true
			visible: delayMethodSelector.currentIndex == 0

			SpinBox {
				from: 0
				to: 1500
				Layout.fillWidth: true
			}

			Label {
				text: " cycles"
			}
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

			SpinBox {
				from: 0
				to: 1500
				Layout.fillWidth: true
			}

			Label {
				text: " - "
			}

			SpinBox {
				from: 0
				to: 1500
				Layout.fillWidth: true
			}

			Label {
				text: " cycles"
			}
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

			SpinBox {
				from: 0
				to: 1500
				Layout.fillWidth: true
			}

			Label {
				text: " cycles/frame"
			}
		}

		Item { Layout.fillHeight: true }
		Item { Layout.fillHeight: true }
	}
}
