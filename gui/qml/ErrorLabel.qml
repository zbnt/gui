
import QtQuick 2.12
import QtQuick.Controls 2.3

import zbnt 1.0

Label {
	property bool valid: true
	property string normalText: ""
	property string errorText: ""

	color: !enabled ? DisabledLabel.color : (valid ? DefaultLabel.color : "#e53935")
	text: valid ? normalText : errorText
	font.italic: true
}
