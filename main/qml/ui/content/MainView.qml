import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../../../../../../../games"

Item {
	id: root

	property real blurAmount: 0

	function scheduleUpdate() {
		mainContentSource.scheduleUpdate();
	}

	anchors.fill: parent

	ListView {
		id: gameList
		anchors.fill: parent
		orientation: Qt.Horizontal
		snapMode: ListView.SnapOneItem
		highlightRangeMode: ListView.StrictlyEnforceRange
		model: GamesList {}
		delegate: GameDelegate {}
	}

	FastBlur {
		anchors.fill: gameList
		radius: root.blurAmount
		visible: root.blurAmount
		source: ShaderEffectSource {
			id: mainContentSource
			anchors.fill: parent
			sourceItem: gameList
			hideSource: false
			live: false
			visible: root.blurAmount
		}
	}
}
