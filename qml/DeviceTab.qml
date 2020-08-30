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
import QtQuick.Dialogs 1.3

import zbnt 1.0

Item {
	id: root

	property bool changePending: false
	property bool discoveryBusy: true

	Connections {
		target: ZBNT

		onRunningChanged: {
			root.changePending = false;
		}

		onDiscoveryDone: {
			root.discoveryBusy = false;
		}

		onConnectionError: {
			errorDialog.text = "Failed to connect to device : " + error;
			errorDialog.open();
		}
	}

	MessageDialog {
		id: errorDialog
		title: "Error"
		text: ""
		icon: StandardIcon.Critical
		standardButtons: StandardButton.Ok
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		GroupBox {
			title: "Connection"
			topPadding: 26
			bottomPadding: 15

			label: Label {
				y: 5
				text: parent.title
				font.weight: Font.Bold
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			GridLayout {
				columns: 2
				rowSpacing: 5
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Device:"
					font.weight: Font.Bold
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				}

				ComboBox {
					id: deviceSelector
					model: ZBNT.deviceList
					enabled: ZBNT.connected == ZBNT.Disconnected

					Layout.fillWidth: true

					delegate: ItemDelegate {
						text: modelData.hostname + " (v" + modelData.versionstr + " at " + modelData.fullAddr + ")"
						width: parent.width

						onClicked: {
							deviceSelector.currentIndex = index
							deviceSelector.displayText = text
						}
					}

					onCurrentIndexChanged: {
						if(currentIndex == -1)
						{
							displayText = ""
						}
						else
						{
							var activeItem = ZBNT.deviceList[deviceSelector.currentIndex]
							displayText = activeItem.hostname + " (v" + activeItem.versionstr + " at " + activeItem.fullAddr + ")"
						}
					}
				}

				Item {
					Layout.fillHeight: true
				}

				RowLayout {
					Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

					Button {
						text: "Rescan"
						enabled: ZBNT.connected == ZBNT.Disconnected && !root.discoveryBusy
						focusPolicy: Qt.NoFocus

						Layout.alignment: Qt.AlignRight

						onClicked: {
							ZBNT.scanDevices()
							root.discoveryBusy = true
						}
					}

					Button {
						text: ["Connect", "Connecting", "Disconnect"][ZBNT.connected]
						enabled: ZBNT.connected != ZBNT.Connecting && !ZBNT.running
						focusPolicy: Qt.NoFocus

						Layout.alignment: Qt.AlignRight

						onClicked: {
							if(ZBNT.connected == ZBNT.Disconnected)
							{
								if(ZBNT.deviceList.length == 0 || deviceSelector.currentIndex == -1)
								{
									errorDialog.text = "No device selected, try rescanning the network.";
									errorDialog.open();
								}
								else
								{
									ZBNT.connectToBoard(ZBNT.deviceList[deviceSelector.currentIndex].ip, ZBNT.deviceList[deviceSelector.currentIndex].port);
								}
							}
							else
							{
								ZBNT.disconnectFromBoard();
							}
						}
					}
				}
			}
		}

		GroupBox {
			title: "Settings"
			topPadding: 26
			bottomPadding: 15

			label: Label {
				y: 5
				text: parent.title
				font.weight: Font.Bold
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			DeviceSettings {
				id: settingsGeneral

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5
			}
		}

		GroupBox {
			title: "Status"
			topPadding: 26
			bottomPadding: 15

			label: Label {
				y: 5
				text: parent.title
				font.weight: Font.Bold
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			ColumnLayout {
				spacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				RowLayout {
					spacing: 10

					Layout.alignment: Qt.AlignHCenter

					Label {
						text: "Time:"
						font.weight: Font.Bold
						horizontalAlignment: Text.AlignRight
					}

					Label {
						text: ZBNT.cyclesToTime(ZBNT.currentTime)
					}
				}

				ProgressBar {
					from: 0
					to: 2048
					value: ZBNT.currentProgress
					Layout.fillWidth: true
				}

				Button {
					text: ZBNT.running ? "Stop" : "Start"
					enabled: ZBNT.connected == ZBNT.Connected && !root.changePending
					focusPolicy: Qt.NoFocus

					Layout.alignment: Qt.AlignRight

					onClicked: {
						if(!ZBNT.running)
						{
							ZBNT.startRun(ZBNT.exportResults)
						}
						else
						{
							ZBNT.stopRun()
						}

						root.changePending = true
					}
				}
			}
		}

		Item {
			Layout.fillHeight: true
		}
	}
}
