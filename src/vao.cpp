/*
 * Vao.cpp
 *
 *  Created on: Mar 31, 2012
 *      Author: cgibson
 */
#include <GL/glew.h>

#include <stdio.h>
#include "vao.hpp"

Vao::Vao()
{

}


Vao::~Vao()
{

}


void
Vao::setShaderProgram(GLuint prog)
{
	shader_prog = prog;
}


void
Vao::create(GLuint target,
			GLuint size,
			void * ptr,
			GLuint usageType)
{
	this->target = target;

	// Create handles
	glGenBuffers(1, &vbo_handle);
	glGenVertexArrays(1, &vao_handle);

	// Bind out vertex buffer object
	glBindBuffer(target, vbo_handle);

	// Populate the vertex buffer object with our data
	glBufferData(target, size, ptr, usageType);

	// Reset
	glBindBuffer(target, 0);

	// Now we're done.  The user needs to run bindAttribute to add
	// attributes to our buffer.
}


void
Vao::bindAttribute(const char * name,
				   GLuint size,
				   GLuint dataType,
				   GLuint normalized,
				   GLuint stride,
				   GLvoid *offset)
{
	// Enable the array
	glUseProgram(shader_prog);
	GLint attribLoc = glGetAttribLocation(shader_prog, name);

	// Bind the current vertex buffer object
	glBindVertexArray(vao_handle);
	glBindBuffer(target, vbo_handle);

	// Enable this attribute, letting opengl know our intent
	glEnableVertexAttribArray(attribLoc);

	// Bind the buffer to the attribute
	glVertexAttribPointer(attribLoc, size, dataType, normalized, stride, offset);

	// Reset
	glBindBuffer(target, 0);
	glBindVertexArray(0);

}


void
Vao::draw(GLuint mode, GLuint first, GLuint count)
{
	// Begin using the given shader
	glUseProgram(shader_prog);

	// Bind the current vertex array object
	glBindVertexArray(vao_handle);

	// Draw the objects we want
	glDrawArrays(mode, first, count);

	// Reset
	glBindVertexArray(0);
	glUseProgram(0);
}
