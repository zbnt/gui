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

Item {
	id: root

	property bool settingsValid: false

	Component.onCompleted: {
		ZBNT.ip = Qt.binding(function() { return deviceSelector.currentIndex != -1 ? ZBNT.deviceList[deviceSelector.currentIndex].ip : "" })
	}

	MessageDialog {
		id: errorDialog
		title: "Error"
		text: ""
		icon: StandardIcon.Critical
		standardButtons: StandardButton.Ok
	}

	Connections {
		target: ZBNT

		onConnectionError: {
			errorDialog.text = "Error in connection to " + ZBNT.ip + " : " + error;
			errorDialog.open();
		}
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
						text: modelData.hostname + " (v" + modelData.version + " at " + modelData.ip + ")"
						enabled: modelData.version == ZBNT.networkVersion
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
							displayText = activeItem.hostname + " (v" + activeItem.version + " at " + activeItem.ip + ")"
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
						enabled: ZBNT.connected == ZBNT.Disconnected
						focusPolicy: Qt.NoFocus

						Layout.alignment: Qt.AlignRight

						onClicked: {
							ZBNT.scanDevices()
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
								if(!deviceSelector.currentIndex == -1)
								{
									errorDialog.text = "No device selected, try rescanning the network.";
									errorDialog.open();
								}
								else if(ZBNT.deviceList[deviceSelector.currentIndex].version > ZBNT.networkVersion)
								{
									errorDialog.text = "The selected device is running a newer version of the network code.";
									errorDialog.open();
								}
								else if(ZBNT.deviceList[deviceSelector.currentIndex].version < ZBNT.networkVersion)
								{
									errorDialog.text = "The selected device is running an older version of the network code.";
									errorDialog.open();
								}
								else
								{
									ZBNT.connectToBoard();
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
			title: "Status"
			topPadding: 26
			bottomPadding: 15

			label: Label {
				y: 5
				text: parent.title
				verticalAlignment: Text.AlignTop
				horizontalAlignment: Text.AlignHCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}

			Layout.fillWidth: true

			GridLayout {
				columns: 2
				rowSpacing: 10
				columnSpacing: 10

				anchors.fill: parent
				anchors.topMargin: 5
				anchors.leftMargin: 5
				anchors.rightMargin: 5

				Label {
					text: "Time:"
					font.weight: Font.Bold
					horizontalAlignment: Text.AlignRight
					Layout.minimumWidth: parent.width / 2
				}

				Label {
					text: ZBNT.currentTime
					Layout.fillWidth: true
				}

				ProgressBar {
					from: 0
					to: 2048
					value: ZBNT.currentProgress
					Layout.columnSpan: 2
					Layout.fillWidth: true
				}

				Button {
					text: ZBNT.running ? "Stop" : "Start"
					enabled: ZBNT.connected == ZBNT.Connected
					focusPolicy: Qt.NoFocus

					Layout.columnSpan: 2
					Layout.alignment: Qt.AlignRight

					onClicked: {
						if(!ZBNT.running)
						{
							if(!root.settingsValid)
							{
								errorDialog.text = "Invalid configuration, please correct the errors and try again.";
								errorDialog.open()
							}
							else
							{
								ZBNT.startRun();
							}
						}
						else
						{
							ZBNT.stopRun();
						}
					}
				}
			}
		}

		Item {
			Layout.fillHeight: true
		}
	}
}
