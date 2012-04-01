/*
 * vao.hpp
 *
 *  Created on: Mar 31, 2012
 *      Author: cgibson
 */

#ifndef VAO_HPP_
#define VAO_HPP_

#include <vector>

using std::vector;

class Vao {
public:
	Vao();
	virtual ~Vao();

	void setShaderProgram(GLuint prog);

	void create(GLuint target,
			    GLuint size,
				void *ptr,
				GLuint usageType);

	void bindAttribute(const char * name,
					   GLuint size,
					   GLuint dataType,
					   GLuint normalized,
					   GLuint stride,
					   GLvoid *offset);

	void draw(GLuint mode, GLuint first, GLuint count);

private:
	GLuint target;
	GLuint shader_prog;
	GLuint vbo_handle;
	GLuint vao_handle;
};


#endif /* VAO_HPP_ */
