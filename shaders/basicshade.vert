#version 400

in vec3 VertexPosition;
in vec3 VertexNormal;

uniform mat4 MVP;
uniform mat3 NormalMatrix;

out vec3 Normal;

void main() {
    Normal = normalize( NormalMatrix * VertexNormal );
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}