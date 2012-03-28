#version 400

in vec3 VertexPosition;
in vec2 VertexTexCoord;

out vec2 TexCoord;

void main() {
	TexCoord = VertexTexCoord;
	gl_Position = vec4(VertexPosition, 1.0);
}