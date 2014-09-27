import QtQuick 2.0

Item {
	id: gameDelegate
	property url backgroundImage: model.backgroundImage
	property bool current: ListView.view.currentIndex == model.index && !ListView.view.dragging

	width: ListView.view.width
	height: ListView.view.height

	Image {
		id: background
		anchors.fill: parent
		fillMode: Image.PreserveAspectCrop
		source: model.backgroundImage
	}

	Item {
		id: game
		anchors.fill: parent
		opacity: 0

		Item {
			anchors { fill: parent; margins: settings.screenMargin }
			Text {
				id: titleText
				elide: Text.ElideRight
				text: model.title
				color: "white"
				style: Text.Raised
				styleColor: "#444"
				font { pixelSize: settings.fontL; bold: true }
				anchors {
					left: parent.left; right: parent.right; bottom: parent.bottom
				}
				transform: Translate {
					id: titleTranslate
					x: -gameDelegate.width
				}
			}
		}

		Button {
			id: startButton
			anchors.centerIn: parent
			text: "Start"
			effectsOn: true
			onClicked: {
				emulatorView.show();
				app.startGame(model.cartridge);
			}
		}
	}

	states: [
		State {
			name: "default"
			when: !current
			PropertyChanges {
				target: game
				opacity: 0
			}
			PropertyChanges {
				target: startButton
				opacity: 0
			}
			PropertyChanges {
				target: titleTranslate
				x: -gameDelegate.width
			}
		},
		State {
			name: "current"
			when: current
			PropertyChanges {
				target: game
				opacity: 1
			}
			PropertyChanges {
				target: startButton
				opacity: 1
			}
			PropertyChanges {
				target: titleTranslate
				x: 0
			}
		}
	]

	transitions: [
		Transition {
			to: "default"
			SequentialAnimation {
				PropertyAnimation {
					target: game
					duration: 100
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
					properties: "opacity,x,y"
				}
				PauseAnimation {
					duration: 500
				}
				PropertyAnimation {
					target: titleTranslate
					duration: 1000
					properties: "x"
				}
				PropertyAnimation {
					target: startButton
					duration: 500
					properties: "opacity"
				}
			}
		}
	]
}
