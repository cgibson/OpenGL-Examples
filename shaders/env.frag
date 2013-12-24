#version 400

in vec3 Normal;
in vec3 ViewDir;
in vec3 WorldNorm;

uniform samplerCube Tex1;

out vec4 FragColor;

void main() {
    
    float transparencyAmt = 0.0;
    
    float refractContrib = 0.0;
    float reflectContrib = 0.8;
    vec4 diffuse = vec4(1.0);
	
	
    vec3 ReflectDir = reflect(-ViewDir, WorldNorm);
    vec3 RefractDir = refract(-ViewDir, WorldNorm, 1.2);
	float intensity = max(0.3, dot(Normal, vec3(1,1,1)));
	vec4 color = diffuse * intensity;
	
	vec4 reflectColor = texture( Tex1, ReflectDir);
	vec4 refractColor = texture( Tex1, RefractDir);
	vec4 transparentColor = texture( Tex1, ViewDir);
    
    if (length(RefractDir) < 0.1) {
        reflectContrib = reflectContrib + refractContrib;
        refractContrib = 0.0;
    }
    
    vec4 rflColor = mix(reflectColor, refractColor, refractContrib / (reflectContrib + refractContrib));
	
	FragColor = mix(mix(rflColor * diffuse, color, 1 - (reflectContrib + refractContrib)), transparentColor, transparencyAmt);
}