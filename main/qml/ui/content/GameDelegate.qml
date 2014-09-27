import QtQuick 2.0

Item {
	id: game
	property url backgroundImage: model.backgroundImage
	property string title: model.title
	property bool current: ListView.view.currentIndex == model.index && !ListView.view.dragging

	width: ListView.view.width
	height: ListView.view.height
	opacity: 0

	Rectangle {
		id: backgroundDim
		anchors { fill: parent; margins: settings.margin }
		color: "black"
		opacity: 0
	}

	Item {
		anchors { fill: parent; margins: settings.screenMargin }
		Text {
			id: titleText
			elide: Text.ElideRight
			text: game.title
			color: "white"
			style: Text.Raised
			styleColor: "#444"
			font { pixelSize: settings.fontL; bold: true }
			anchors {
				top: parent.top; left: parent.left; right: parent.right
			}
		}
	}

	states: [
		State {
			name: "default"
			when: !current
			PropertyChanges {
				target: backgroundDim
				opacity: 0
			}
			PropertyChanges {
				target: game
				opacity: 0
			}
		},
		State {
			name: "current"
			when: current
			PropertyChanges {
				target: backgroundDim
				opacity: 0.5
			}
			PropertyChanges {
				target: game
				opacity: 1
			}
		}
	]

	transitions: [
		Transition {
			to: "default"
			SequentialAnimation {
				PropertyAnimation {
					target: game
					duration: 150
					properties: "opacity"
				}
				PropertyAction {
					properties: "opacity"
				}
			}
		},
		Transition {
			to: "current"
			SequentialAnimation {
				PropertyAction {
					target: game
					properties: "opacity"
				}
				PropertyAnimation {
					target: backgroundDim
					duration: 500
					properties: "opacity"
				}
			}
		}
	]
}
