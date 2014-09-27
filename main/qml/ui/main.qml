import QtQuick 2.0
import "content"
//import generator 1.0

Rectangle {
	id: mainWindow

	width: 800; height: 480

	QtObject {
		id: settings
		// These are used to scale fonts according to screen size
		property real _scaler: 300 + mainWindow.width * mainWindow.height * 0.00015
		property int fontXS: _scaler * 0.032
		property int fontS: _scaler * 0.040
		property int fontM: _scaler * 0.046
		property int fontMM: _scaler * 0.064
		property int fontL: _scaler * 0.100

		property int margin: _scaler * 0.046
		property int screenMargin: _scaler * 0.064
	}

	MainView {
		id: mainView
	}

	EmulatorView {
		id: emulatorView
	}
}
