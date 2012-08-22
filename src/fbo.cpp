#include "fbo.hpp"

#include "GL/gl.h"

#include <stdlib.h>
#include <stdio.h>

/*
 * FBO Helper constructor
 */
Fbo::Fbo()
{
	reset();
}

/*
 * Initialize the FBO object and generate textures for render targets
 */
bool Fbo::create( GLuint width, GLuint height, GLuint count )
{
	reset();

	this->width = width;
	this->height = height;
	this->texture_count = count;

	// Allocate enough space for the handles and their buffer information
	texture_handles = (GLuint*)malloc( sizeof(GLuint*) * texture_count);
	GLenum bufs[texture_count];

	// Initialize the FBO itself
	bool r = initializeFBO( true );

	if (!r) {
	    printf("Initialization failed\n");
	}

	// bind current FBO
	glBindFramebuffer( GL_FRAMEBUFFER, fbo_handle );

	// for every texture, generate a texture and add it to the FBO
	for( int i = 0; i < texture_count; i++ )
	{
		// generate the new texture at i
		r &= generateTexture(width, height, &texture_handles[i]);
		bufs[i] = GL_COLOR_ATTACHMENT0 + i;

		// attach texture to currently bound FBO as color attachment i
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
								   GL_TEXTURE_2D, texture_handles[i], 0);
	}

	glDrawBuffers( texture_count, bufs);

	// Unbind current FBO
	glBindFramebufferEXT( GL_FRAMEBUFFER, 0 );

	// Check for errors
	GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER );
	if( status != GL_FRAMEBUFFER_COMPLETE )
	{
		reset();
		return false;
	}

	return r;
}

/*
 * Internal function to initialize the FBO object
 */
bool Fbo::initializeFBO(bool useDepthBuffer )
{

	// create new FBO handle and bind it
	glGenFramebuffers( 1, &fbo_handle );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo_handle );

	// attach depth buffer if we need it
	if( useDepthBuffer )
	{
		// Generate and bind the buffer
		glGenRenderbuffers( 1, &depth_handle );
		glBindRenderbuffer( GL_RENDERBUFFER, depth_handle );

		// Identify that this is a depth component and its size
		glRenderbufferStorage(GL_RENDERBUFFER,
							  GL_DEPTH_COMPONENT,
							  width, height );

		// attach depth render buffer to our FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
								  GL_DEPTH_ATTACHMENT,
								  GL_RENDERBUFFER,
								  depth_handle );
	}

	// check for errors and report them
	/*GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if( status != GL_FRAMEBUFFER_COMPLETE )
	{
		reset();
		return false;
	}*/

	// reset current bound buffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	return true;
}

bool Fbo::generateTexture( GLuint width, GLuint height, GLuint *handle)
{
	// Generate the texture handle
	glGenTextures(1, handle);

	// Set the current active texture (let's choose zero)
	glActiveTexture(GL_TEXTURE0);

	// Bind our current texture handle as the active texture
	glBindTexture(GL_TEXTURE_2D, *handle);

	// Create an RGBA texture of the width and height desired
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, NULL);

	// Set some basic parameters for texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	return true;
}

/*
 * Enable the current FBO object
 */
bool Fbo::enable()
{
	// Only run if we are not currently enabled, and we have a valid fbo
	if( !enabled && fbo_handle )
	{
		// Bind our current framebuffer
		glBindFramebuffer( GL_FRAMEBUFFER, fbo_handle );

		// Assign the viewport to the correct size
		glViewport( 0, 0, width, height );

		// Finally mark as enabled
		enabled = true;
	}

	// Return the current enabled bit otherwise
	return enabled;
}

/*
 * Disable the current FBO object
 */
bool Fbo::disable()
{
	// Only run if we were previously enabled
	if( enabled )
	{

	    //glGenerateMipmap(GL_TEXTURE_2D);
		// Reset to the original state (no frame buffer)
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		enabled = false;
	}

	// Return current (expectedly false) status
	return enabled;
}

/*
 * Reset all values
 */
void Fbo::reset()
{
	disable();

	// if depth buffer exists
	if( depth_handle )
		glDeleteRenderbuffers( 1, &depth_handle);

	// if FBO object exists
	if( fbo_handle )
		glDeleteRenderbuffers( 1, &fbo_handle );

	width = 0;
	height = 0;

	fbo_handle = -1;
	depth_handle = -1;

	texture_handles = NULL;
	texture_count = 0;

	enabled = false;
}
