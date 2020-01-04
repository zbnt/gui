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

	property var stackLayoutObjects: []

	Connections {
		target: ZBNT

		onBitstreamDevicesChanged: {
			var deviceDescriptions = []
			var idx = 0

			for(var obj of root.stackLayoutObjects)
			{
				obj.destroy()
			}

			root.stackLayoutObjects = []

			for(var dev of devices)
			{
				deviceDescriptions.push(dev.description())
				root.stackLayoutObjects.push(Qt.createComponent(dev.settingsQml()).createObject(stackLayout, {object: dev, idx: idx}))
				idx += 1
			}

			categorySelector.model = deviceDescriptions
		}
	}

	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		ComboBox {
			id: categorySelector
			Layout.fillWidth: true
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
				id: stackLayout
				anchors.fill: parent
				currentIndex: categorySelector.currentIndex
			}
		}
	}
}
