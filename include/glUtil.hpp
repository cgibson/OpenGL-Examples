/*
 * glutil.h
 *
 *  Created on: Mar 10, 2012
 *      Author: cgibson
 */

#ifndef GLUTIL_H_
#define GLUTIL_H_

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw.h>


void printGLVersion()
{

	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor            : %s\n", vendor);
	printf("GL Renderer          : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);

}


#endif /* GLUTIL_H_ */
