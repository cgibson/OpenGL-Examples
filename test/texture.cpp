//========================================================================
// In this example, we use our imageUtil.cpp code (using DeVIL)
// to load and display a texture on a quad
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "glm/glm.hpp"

#include "GLSLProgram.hpp"
#include "imageUtil.hpp"
#include "glUtil.hpp"

#define BUFFER_OFFSET(i) ((GLfloat*)NULL + (i))

typedef struct CVertex
{
	GLfloat pos[3];
	GLfloat uv[2];
} CVertex;

int main( void )
{
    int width, height, x;

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
    
    glewExperimental = GL_TRUE;

    // We need this to get the code to compile+run on MacOSX. I have yet
    // to confirm if this works on Linux...

#ifdef __APPLE__
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( 640, 480, 8,8,8,8,24,8, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );

        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    // Initialize GLEW
    GLenum err = glewInit();
    if( err != GLEW_OK )
    {
        fprintf( stderr, "Failed to initialize GLEW: %s\n",
        				 glewGetErrorString(err));
        exit( EXIT_FAILURE );
    }

    printGLVersion();

    fprintf( stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	if (!GLEW_ARB_vertex_buffer_object)
	{
        fprintf( stderr, "VBO not supported\n");
		exit( EXIT_FAILURE );
	}

    glfwSetWindowTitle( "Spinning Triangle" );

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );

    shader::GLSLProgram prog;

    // Compile vertex shader
    if( ! prog.compileShaderFromFile("shaders/basictexture.vert", shader::VERTEX))
    {
		printf("Vertex shader failed to compile!\n%s", prog.log().c_str());
		exit(1);
	}

    // Compile fragment shader
    if( ! prog.compileShaderFromFile("shaders/basictexture.frag", shader::FRAGMENT))
    {
		printf("Fragment shader failed to compile!\n%s", prog.log().c_str());
		exit(1);
	}

    // Link shaders
    if( ! prog.link() )
    {
    	printf("Shader program failed to link!\n%s", prog.log().c_str());
    	exit(1);
    }

    GLint pLoc   = prog.getAttribLocation("VertexPosition");
    GLint uvLoc   = prog.getAttribLocation("VertexTexCoord");

    prog.setUniform("Tex1", 0);

    GLuint vaoHandle;
    CVertex packedData[] = {
    		{{ 0.8f, -0.8f, 0.0f},
    		 { 1.0f,  0.0f}},

    		{{-0.8f, -0.8f, 0.0f},
    		 { 0.0f,  0.0f}},

    		{{ 0.8f,  0.8f, 0.0f},
    		 { 1.0f,  1.0f}},

    		{{-0.8f,  0.8f, 0.0f},
    		 { 0.0f,  1.0f}}
    };

    void * somePtr = (void*)packedData;

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    GLuint dataBufferHandle = vboHandle;;

    // Populate position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(CVertex), somePtr,
    			 GL_STATIC_DRAW);


    // Create and set-up array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(pLoc); // Vertex position
    glEnableVertexAttribArray(uvLoc); // Vertex color

    // Map index 0 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);

    glVertexAttribPointer( pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0) );
    glVertexAttribPointer( uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3) );

    // REMEMBER THESE!!!
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);

    GLuint texID = util::image::loadImage("img/random.png");
    
    printf("Texture: %d\n", texID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    do
    {
        glfwGetMousePos( &x, NULL );

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        prog.use();
        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
