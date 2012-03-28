#version 400

in vec2 TexCoord;

uniform sampler2D Tex1;

out vec4 FragColor;

void main() {
	vec4 texColor = texture( Tex1, TexCoord );
	
	FragColor = texColor;
}