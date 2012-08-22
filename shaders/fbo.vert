#version 400

in vec3 VertexPosition;
in vec3 VertexUV;

out vec3 UV;
uniform mat4 MVP;

void main() {
    UV = VertexUV;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}