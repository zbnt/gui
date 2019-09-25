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
	ColumnLayout {
		anchors.fill: parent
		spacing: 10

		TrafficTabBox {
			title: "eth0"
			object: ZBNT.sc0
		}

		TrafficTabBox {
			title: "eth1"
			object: ZBNT.sc1
		}

		TrafficTabBox {
			title: "eth2"
			object: ZBNT.sc2
		}

		TrafficTabBox {
			title: "eth3"
			object: ZBNT.sc3
		}

		Item {
			Layout.fillHeight: true
		}
	}
}
