/*
 * glslHelper.h
 *
 *  Created on: Mar 10, 2012
 *      Author: cgibson
 */

#ifndef GLSLHELPER_H_
#define GLSLHELPER_H_

#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string.h>

#include <fstream>

using namespace std;

std::string loadFileToString(char const * const fname)
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

GLuint
compileShader( string filename )
{
	// Grab the file's extension
	string fileExt = filename.substr(filename.find_last_of(".") + 1);

	// Identify if this is a fragment shader or a vertex shader
	GLuint shaderType;
	if (fileExt == "vert")
		shaderType = GL_VERTEX_SHADER;
	else if (fileExt == "frag")
		shaderType = GL_FRAGMENT_SHADER;
	else
	{
        fprintf( stderr, "Unable to load shader file %s\n", filename.c_str() );
        return -1;
	}

    // Build the shader based on its type
	GLuint shader = glCreateShader( shaderType );
	if ( shader == 0 )
	{
		fprintf( stderr, "Error creating shader for %s.\n", filename.c_str() );
		return -1;
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
		fprintf( stderr, "Compilation of %s shader failed!\n", fileExt.c_str() );

		GLint logLen;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLen );

		if( logLen > 0 )
		{
			char * log = (char *)malloc( logLen );
			GLsizei written;
			glGetShaderInfoLog( shader, logLen, &written, log);

			fprintf( stderr, "Shader Log:\n%s\n", log);
			free( log );
		}

		return -1;
	}

	return shader;
}

GLuint
createGLSLProgram( string shader )
{
	// Compile vertex shader
	GLuint vertShader = compileShader( shader + ".vert" );
	if (!vertShader)
		return -1;

	// Compile fragment shader
	GLuint fragShader = compileShader( shader + ".frag" );
	if (!fragShader)
		return -1;

	// Create the shader program
	GLuint program = glCreateProgram();
	if( !program )
	{
		fprintf(stderr, "Error creating program object.\n");
		return -1;
	}

	// Attach the shaders to the program
	glAttachShader( program, vertShader);
	glAttachShader( program, fragShader);
	glLinkProgram( program );

	// Check for linking errors
	GLint result;
	glGetProgramiv( program, GL_LINK_STATUS, &result );
	if( result == GL_FALSE )
	{
		fprintf( stderr, "Linking of %s shader failed!\n", shader.c_str() );

		GLint logLen;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLen );

		if( logLen > 0 )
		{
			char * log = (char *)malloc( logLen );
			GLsizei written;
			glGetProgramInfoLog( program, logLen, &written, log);

			fprintf( stderr, "Program Log:\n%s\n", log);
			free( log );
		}

		return -1;
	}
	else
	{
		//glUseProgram( program );
	}

	return program;
}


#endif /* GLSLHELPER_H_ */
