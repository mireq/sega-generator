import QtQuick 2.0
import generator 1.0

Item {
	id: root

	anchors.fill: parent
	opacity: 0
	visible: opacity
	scale: 0.3

	property bool isShown: false

	function show() {
		mainView.scheduleUpdate();
		root.isShown = true;
		showAnimation.restart();
	}
	function hide() {
		hideAnimation.restart();
	}

	Binding {
		target: mainView
		property: "blurAmount"
		value: 40 * root.opacity
		when: root.isShown
	}

	ParallelAnimation {
		id: showAnimation
		NumberAnimation { target: root; property: "opacity"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
		NumberAnimation { target: root; property: "scale"; to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
	}
	SequentialAnimation {
		id: hideAnimation
		ParallelAnimation {
			NumberAnimation { target: root; property: "opacity"; to: 0; duration: 500; easing.type: Easing.InOutQuad }
			NumberAnimation { target: root; property: "scale"; to: 0.3; duration: 500; easing.type: Easing.InOutQuad }
		}
		PropertyAction { target: root; property: "isShown"; value: false }
	}

	Rectangle {
		id: backgroundItem
		anchors.centerIn: parent
		width: 642
		height: 450
		border.color: "#808080"
		border.width: 1
		opacity: 0.9
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#101010" }
			GradientStop { position: 0.3; color: "#404040" }
			GradientStop { position: 1.0; color: "#090909" }
		}
		EmulatorItem {
			anchors.fill: parent
			anchors.margins: 1
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			hide();
			app.stopGame();
		}
	}
}
