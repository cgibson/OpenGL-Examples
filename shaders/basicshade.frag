#version 400

in vec3 Normal;

out vec4 FragColor;

void main() {
    vec3 LightDir = vec3(1, 1, 1);
    LightDir = normalize( LightDir );
    
    vec3 Color = vec3(1,1,1);
    float Intensity = dot(Normal, LightDir);
    Color = Color * Intensity;
	FragColor = vec4(max(vec3(0.1,0.1,0.1), Color), 1.0);
}