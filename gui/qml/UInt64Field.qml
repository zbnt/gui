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

import zbnt 1.0

TextField {
	id: root

	property string min: "0"
	property string max: "9223372036854775807"

	text: min
	maximumLength: Math.max(1, max.length)
	horizontalAlignment: Qt.AlignHCenter

	color: !enabled ? DisabledLabel.color : (valid ? DefaultLabel.color : "#e53935")
	property bool valid: validator.validate(text)

	validator: UInt64Validator {
		top: root.max
		bottom: root.min
	}

	onMinChanged: {
		onTextChanged(text)
	}

	onMaxChanged: {
		onTextChanged(text)
	}
}
