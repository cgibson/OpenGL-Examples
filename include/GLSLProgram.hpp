#ifndef GLSLPROGRAM_HPP_
#define GLSLPROGRAM_HPP_

#include <string>
#include <GL/glew.h>
#include <GL/glfw.h>

#include <glm/glm.hpp>

using namespace glm;
using std::string;

namespace shader {

enum GLSLShaderType {
	VERTEX, FRAGMENT, GEOMETRY, TESS_CONTROL, TESS_EVALUATION
};

class GLSLProgram
{
public:
	GLSLProgram();

	bool compileShaderFromFile( const string & filename, GLSLShaderType type );

	bool link();
	bool use();
	string log();

	int getHandle() { return handle; }
	bool isLinked() { return linked; }

	void bindAttribLocation( GLuint location, const char * name );
	void bindFragDataLocation( GLuint location, const char * name );

	GLuint getAttribLocation(const char * name);

	void setUniform(const char * name, float x, float y, float z);
	void setUniform(const char * name, const vec3 & v);
	void setUniform(const char * name, const vec4 & v);
	void setUniform(const char * name, const mat3 & m);
	void setUniform(const char * name, const mat4 & m);
	void setUniform(const char * name, float val);
	void setUniform(const char * name, int val);
	void setUniform(const char * name, bool val);

private:
	GLuint getUniformLocation(const char * name);
	string loadFileToString(char const * const fname);

	int handle;
	bool linked;
	string logString;
};

}

#endif /* GLSLPROGRAM_HPP_ */
