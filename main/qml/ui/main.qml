import QtQuick 2.0
import QtGraphicalEffects 1.0
import generator 1.0

Rectangle {
	width: 800; height: 480
	EmulatorItem {
		id: emulator
		width: 640; height: 448
	}

	ShaderEffectSource {
		id: sourceProxy
		sourceItem: emulator
		live: true
		hideSource: true
	}


	ShaderEffect {
		width: 640; height: 448
		property variant source: sourceProxy
		vertexShader: "
			uniform lowp mat4 qt_Matrix;
			attribute lowp vec4 qt_Vertex;
			attribute lowp vec2 qt_MultiTexCoord0;
			varying lowp vec2 qt_TexCoord0;
			varying lowp vec2 qt_TexCoord1;
			varying lowp vec2 qt_TexCoord2;
			varying lowp vec2 qt_TexCoord3;
			varying lowp vec2 qt_TexCoord4;
			void main() {
				lowp vec2 rubyTextureSize;
				rubyTextureSize.x = 320.0;
				rubyTextureSize.y = 224.0;
				lowp vec2 offsetx;
				lowp vec2 offsety;
				gl_Position = qt_Matrix * qt_Vertex;
				offsetx.x = 1.0 / rubyTextureSize.x;
				offsetx.y = 0.0;
				offsety.y = 1.0 / rubyTextureSize.y;
				offsety.x = 0.0;
				qt_TexCoord0 = qt_MultiTexCoord0; //center
				qt_TexCoord1 = qt_TexCoord0 - offsetx; //left
				qt_TexCoord2 = qt_TexCoord0 + offsetx; //right
				qt_TexCoord3 = qt_TexCoord0 - offsety; //top
				qt_TexCoord4 = qt_TexCoord0 + offsety; //bottom
			}
		"
		fragmentShader: "
			varying lowp vec2 qt_TexCoord0;
			varying lowp vec2 qt_TexCoord1;
			varying lowp vec2 qt_TexCoord2;
			varying lowp vec2 qt_TexCoord3;
			varying lowp vec2 qt_TexCoord4;

			uniform sampler2D source;
			uniform lowp float qt_Opacity;

			void main() {
				lowp vec2 rubyTextureSize;
				rubyTextureSize.x = 320.0;
				rubyTextureSize.y = 224.0;

				lowp vec4 colD, colF, colB, colH, col, tmp;
				lowp vec2 sel;
				col = texture2D(source, qt_TexCoord0); //central (can be E0-E3)
				colD = texture2D(source, qt_TexCoord1); //D (left)
				colF = texture2D(source, qt_TexCoord2); //F (right)
				colB = texture2D(source, qt_TexCoord3); //B (top)
				colH = texture2D(source, qt_TexCoord4); //H (bottom)
				sel = fract(qt_TexCoord0.xy * rubyTextureSize.xy); //where are we (E0-E3)?
				//E0 is default
				if(sel.y >= 0.5) { tmp = colB; colB = colH; colH = tmp; } //E1 (or E3): swap B and H
				if(sel.x >= 0.5) { tmp = colF; colF = colD; colD = tmp; } //E2 (or E3): swap D and F
				if(colB == colD && colB != colF && colD != colH) { //do the Scale2x rule
					col = colD;
				}
				gl_FragColor = col;
			}
		"
	}
}
