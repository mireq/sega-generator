import QtQuick 2.0

Item {
	id: root

	anchors.fill: parent

	Image {
		id: background
		anchors.fill: parent
		fillMode: Image.PreserveAspectCrop
	}

	Component {
		id: imageFadeComponent
		Image {
			id: fadeImage
			anchors.fill: parent;
			fillMode: Image.PreserveAspectCrop
			NumberAnimation on opacity {
				from: 1
				to: 0
				onRunningChanged: {
					if (!running) {
						fadeImage.destroy();
					}
				}
			}
		}
	}

	ListView {
		id: gameList
		anchors.fill: parent
		orientation: Qt.Horizontal
		snapMode: ListView.SnapOneItem
		highlightRangeMode: ListView.StrictlyEnforceRange
		z: 1
		model: ListModel {
			ListElement {
				title: "Demo"
				backgroundImage: "/home/mirec/background3.png"
			}
			ListElement {
				title: "Sonic"
				backgroundImage: "/home/mirec/Preberanie/62233.jpg"
			}
		}
		delegate: GameDelegate {}
		onCurrentIndexChanged: {
			if (background.source != "") {
				var image = imageFadeComponent.createObject(root, {});
				image.source = background.source;
			}
			background.source = currentItem.backgroundImage;
		}
	}
}
