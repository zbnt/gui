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

ApplicationWindow {
	id: window

	width: 650
	height: 700
	visible: true
	title: "ZBNT GUI"

	minimumWidth: 650
	minimumHeight: 700

	ColumnLayout {
		spacing: 0
		anchors.fill: parent
		anchors.margins: 5

		Item {
			height: 5
		}

		TabBar {
			id: tabBar
			clip: true
			Layout.fillWidth: true

			TabButton {
				text: "Device"
			}

			TabButton {
				text: "Settings"
			}

			TabButton {
				text: "Traffic"
			}

			TabButton {
				text: "Latency"
			}

			background: Item { }
		}

		Frame {
			Layout.fillWidth: true
			Layout.fillHeight: true

			StackLayout {
				anchors.fill: parent
				currentIndex: tabBar.currentIndex

				DeviceTab {
					id: deviceTab
					settingsValid: settingsTab.ready
				}

				SettingsTab {
					id: settingsTab
				}

				TrafficTab {
					id: trafficTab
				}

				LatencyTab {
					id: latencyTab
					object: ZBNT.lm0
				}
			}
		}
	}
}
