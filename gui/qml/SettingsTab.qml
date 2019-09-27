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

	property bool ready: settingsGeneral.ready && settingsTG1.ready && settingsTG2.ready && (ZBNT.streamMode || (ZBNT.bitstreamID == ZBNT.DualTGen ? settingsLM.ready : (settingsTG3.ready && settingsTG4.ready)))

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		ComboBox {
			id: categorySelector
			Layout.fillWidth: true

			model: [
				"General settings",
				"Traffic generator (eth0)",
				"Traffic generator (eth1)",
				"Traffic generator (eth2)",
				"Traffic generator (eth3)",
				"Latency measurer (eth2, eth3)",
				"Frame detector (eth2, eth3)"
			]
		}

		Frame {
			enabled: true
			topPadding: 15
			leftPadding: 10
			rightPadding: 10
			bottomPadding: 15

			Layout.fillWidth: true
			Layout.fillHeight: true

			StackLayout {
				anchors.fill: parent
				currentIndex: categorySelector.currentIndex

				SettingsTabGeneral {
					id: settingsGeneral
					enabled: !ZBNT.running
				}

				SettingsTabTG {
					id: settingsTG1
					object: ZBNT.tg0
					enabled: !ZBNT.running
				}

				SettingsTabTG {
					id: settingsTG2
					object: ZBNT.tg1
					enabled: !ZBNT.running
				}

				SettingsTabTG {
					id: settingsTG3
					object: ZBNT.tg2
					enabled: !ZBNT.running && ZBNT.bitstreamID == ZBNT.QuadTGen
				}

				SettingsTabTG {
					id: settingsTG4
					object: ZBNT.tg3
					enabled: !ZBNT.running && ZBNT.bitstreamID == ZBNT.QuadTGen
				}

				SettingsTabLM {
					id: settingsLM
					object: ZBNT.lm0
					enabled: !ZBNT.running && ZBNT.bitstreamID == ZBNT.DualTGenLM && !ZBNT.streamMode
				}

				SettingsTabFD {
					id: settingsFD
					object: ZBNT.fd0
					enabled: !ZBNT.running && ZBNT.bitstreamID == ZBNT.DualTGenFD && !ZBNT.streamMode
				}
			}
		}
	}
}