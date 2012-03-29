
#include "GLSLProgram.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace shader {

GLSLProgram::GLSLProgram()
{
	handle = glCreateProgram();
	if( !handle )
	{
		logString = "Error creating program object.\n";
		linked = false;
		return;
	}

	linked = false;
	logString = "";
}


bool
GLSLProgram::compileShaderFromFile(const string & filename, GLSLShaderType type)
{
	if (!handle)
		return false;

	// Identify if this is a fragment shader or a vertex shader
	GLuint shaderType;
	switch(type) {
		case VERTEX: shaderType = GL_VERTEX_SHADER; break;
		case FRAGMENT: shaderType = GL_FRAGMENT_SHADER; break;
		case GEOMETRY: shaderType = GL_GEOMETRY_SHADER; break;
		case TESS_CONTROL: shaderType = GL_TESS_CONTROL_SHADER; break;
		case TESS_EVALUATION: shaderType = GL_TESS_EVALUATION_SHADER; break;
		default:
		{
			logString += "Unable to identify shader type for file " + filename + ".\n";
			return -1;
			break;
		}
	}

    // Build the shader based on its type
	GLuint shader = glCreateShader( shaderType );
	if ( shader == 0 )
	{
		logString += "Error creating shader for " + filename + ".\n";
		return false;
	}

	string shaderStr = loadFileToString( filename.c_str() );
	const GLchar * shaderCode = shaderStr.c_str();
	GLint const shaderLength = shaderStr.size();

	// Set the shader's source
	glShaderSource( shader, 1, &shaderCode, &shaderLength );

	// Compile the shader
	glCompileShader( shader );

	// Check for shader compilation errors
	GLint result;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
	if( result == GL_FALSE )
	{

		GLint logLen;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLen );

		if( logLen > 0 )
		{
			char * log = (char *)malloc( logLen );
			GLsizei written;
			glGetShaderInfoLog( shader, logLen, &written, log);

			logString += log;

			free( log );
		}

		return -1;
	}

	glAttachShader( handle, shader );

	return true;
}


bool
GLSLProgram::link()
{
	if (!handle)
		return false;

	glLinkProgram( handle );

	GLint result;
	glGetProgramiv( handle, GL_LINK_STATUS, &result );
	if( result == GL_FALSE )
	{
		GLint logLen;
		glGetProgramiv( handle, GL_INFO_LOG_LENGTH, &logLen );

		if( logLen > 0 )
		{
			char * log = (char *)malloc( logLen );
			GLsizei written;
			glGetProgramInfoLog( handle, logLen, &written, log);

			// Set our current log string to the grabbed text
			logString = log;

			free( log );
		}

		return -1;
	}
	linked = true;

	return true;
}


bool
GLSLProgram::use()
{
	if (linked)
		glUseProgram(handle);

	return linked;
}


string
GLSLProgram::log()
{
	return logString;
}


void
GLSLProgram::bindAttribLocation(GLuint location, const char * name)
{
	glBindAttribLocation(handle, location, name);
}


void
GLSLProgram::bindFragDataLocation(GLuint location, const char * name)
{
	glBindFragDataLocation(handle, location, name);
}


GLuint
GLSLProgram::getAttribLocation(const char * name)
{
	return glGetAttribLocation(handle, name);
}


void
GLSLProgram::setUniform(const char * name, float x, float y, float z)
{
	GLuint loc = getUniformLocation(name);
	glUniform3f(loc, x, y, z);
}


void
GLSLProgram::setUniform(const char * name, const vec3 & v)
{
	GLuint loc = getUniformLocation(name);
	glUniform3f(loc, v.x, v.y, v.z);
}


void
GLSLProgram::setUniform(const char * name, const vec4 & v)
{
	GLuint loc = getUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}


void
GLSLProgram::setUniform(const char * name, const mat3 & m)
{
	GLuint loc = getUniformLocation(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}


void
GLSLProgram::setUniform(const char * name, const mat4 & m)
{
	GLuint loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}


void
GLSLProgram::setUniform(const char * name, float val)
{
	GLuint loc = getUniformLocation(name);
	glUniform1f(loc, val);
}


void
GLSLProgram::setUniform(const char * name, int val)
{
	GLuint loc = getUniformLocation(name);
	glUniform1i(loc, val);
}


void
GLSLProgram::setUniform(const char * name, bool val)
{
	GLuint loc = getUniformLocation(name);
	glUniform1i(loc, val ? 1 : 0);
}


GLuint
GLSLProgram::getUniformLocation(const char * name)
{
	return glGetUniformLocation(handle, name);
}


string
GLSLProgram::loadFileToString(char const * const fname)
{
    std::ifstream ifile(fname);
    std::string filetext;

    while( ifile.good() ) {
        std::string line;
        std::getline(ifile, line);
        filetext.append(line + "\n");
    }

    return filetext;
}

}
