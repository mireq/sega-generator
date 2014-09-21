import QtQuick 2.0
import QtGraphicalEffects 1.0
import generator 1.0

Rectangle {
	width: 800; height: 480
	color: "blue"
	EmulatorItem {
		id: emulator
		width: 336 * 2
		height: 256 * 2
	}

	ShaderEffectSource {
		id: sourceProxy
		sourceItem: emulator
		sourceRect: Qt.rect(16, 16, 640, 448)
		live: true
		hideSource: true
	}


	ShaderEffect {
		width: 640; height: 448
		property variant source: sourceProxy
		vertexShader: "
			uniform highp mat4 qt_Matrix;
			attribute highp vec4 qt_Vertex;
			attribute highp vec2 qt_MultiTexCoord0;
			varying highp vec2 qt_TexCoord0;
			varying highp vec4 qt_TexCoord1;
			varying highp vec4 qt_TexCoord2;
			varying highp vec4 qt_TexCoord3;
			varying highp vec4 qt_TexCoord4;

			void main() {
				float x = 0.5 * (1.0 / 320.0);
				float y = 0.5 * (1.0 / 224.0);
				vec2 dg1 = vec2( x, y);
				vec2 dg2 = vec2(-x, y);
				vec2 dx = vec2(x, 0.0);
				vec2 dy = vec2(0.0, y);
				qt_TexCoord0 = qt_MultiTexCoord0;
				qt_TexCoord1.xy = qt_TexCoord0.xy - dg1;
				qt_TexCoord1.zw = qt_TexCoord0.xy - dy;
				qt_TexCoord2.xy = qt_TexCoord0.xy - dg2;
				qt_TexCoord2.zw = qt_TexCoord0.xy + dx;
				qt_TexCoord3.xy = qt_TexCoord0.xy + dg1;
				qt_TexCoord3.zw = qt_TexCoord0.xy + dy;
				qt_TexCoord4.xy = qt_TexCoord0.xy + dg2;
				qt_TexCoord4.zw = qt_TexCoord0.xy - dx;
				gl_Position = qt_Matrix * qt_Vertex;
			}"
		fragmentShader: "
			varying highp vec2 qt_TexCoord0;
			varying highp vec4 qt_TexCoord1;
			varying highp vec4 qt_TexCoord2;
			varying highp vec4 qt_TexCoord3;
			varying highp vec4 qt_TexCoord4;

			const float mx = 0.325;
			const float k = -0.250;
			const float max_w = 0.25;
			const float min_w =-0.05;
			const float lum_add = 0.25;

			uniform sampler2D source;
			uniform lowp float qt_Opacity;

			void main() {
				vec3 c00 = texture2D(source, qt_TexCoord1.xy).xyz;
				vec3 c10 = texture2D(source, qt_TexCoord1.zw).xyz;
				vec3 c20 = texture2D(source, qt_TexCoord2.xy).xyz;
				vec3 c01 = texture2D(source, qt_TexCoord4.zw).xyz;
				vec3 c11 = texture2D(source, qt_TexCoord0.xy).xyz;
				vec3 c21 = texture2D(source, qt_TexCoord2.zw).xyz;
				vec3 c02 = texture2D(source, qt_TexCoord4.xy).xyz;
				vec3 c12 = texture2D(source, qt_TexCoord3.zw).xyz;
				vec3 c22 = texture2D(source, qt_TexCoord3.xy).xyz;
				vec3 dt = vec3(1.0, 1.0, 1.0);
				float md1 = dot(abs(c00 - c22), dt);
				float md2 = dot(abs(c02 - c20), dt);
				float w1 = dot(abs(c22 - c11), dt) * md2;
				float w2 = dot(abs(c02 - c11), dt) * md1;
				float w3 = dot(abs(c00 - c11), dt) * md2;
				float w4 = dot(abs(c20 - c11), dt) * md1;
				float t1 = w1 + w3;
				float t2 = w2 + w4;
				float ww = max(t1, t2) + 0.0001;
				c11 = (w1 * c00 + w2 * c20 + w3 * c22 + w4 * c02 + ww * c11) / (t1 + t2 + ww);
				float lc1 = k / (0.12 * dot(c10 + c12 + c11, dt) + lum_add);
				float lc2 = k / (0.12 * dot(c01 + c21 + c11, dt) + lum_add);
				w1 = clamp(lc1 * dot(abs(c11 - c10), dt) + mx, min_w, max_w);
				w2 = clamp(lc2 * dot(abs(c11 - c21), dt) + mx, min_w, max_w);
				w3 = clamp(lc1 * dot(abs(c11 - c12), dt) + mx, min_w, max_w);
				w4 = clamp(lc2 * dot(abs(c11 - c01), dt) + mx, min_w, max_w);
				//gl_FragColor.rgb = c11;
				gl_FragColor.rgb = w1 * c10 + w2 * c21 + w3 * c12 + w4 * c01 + (1.0 - w1 - w2 - w3 - w4) * c11;
				gl_FragColor.a = qt_Opacity;
			}"
	}
}
