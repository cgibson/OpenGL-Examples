#version 400

in vec3 VertexPosition;
in vec3 VertexNormal;

uniform mat4 MVP;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform vec3 WorldCameraPosition;

out vec3 Normal;
out vec3 ViewDir;
out vec3 WorldNorm;

void main() {
    vec3 worldPos = vec3( ModelMatrix * vec4(VertexPosition, 1.0) );
    vec3 worldNorm = vec3(ModelMatrix * vec4(VertexNormal, 1.0) );
    vec3 worldView = normalize( WorldCameraPosition - worldPos );
    
    WorldNorm = worldNorm;
    ViewDir = worldView;
    
    Normal = normalize( NormalMatrix * VertexNormal );
    
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}