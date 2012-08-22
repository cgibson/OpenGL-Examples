#version 400

uniform sampler2D FboTexture;

in vec3 UV;

out vec4 FragColor;

void main() {
    FragColor = texture( FboTexture, UV.xy );
}