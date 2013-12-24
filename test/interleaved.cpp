//========================================================================
// Vertex Array Objects can also be initialized with 'interleaved'
// data (where multiple attributes are defined in a single buffer).
// For basic VAO examples, check out quad.cpp or ring.cpp
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "glm/glm.hpp"

#include "GLSLProgram.hpp"
#include "vao.hpp"
#include "glUtil.hpp"

#define BUFFER_OFFSET(i) ((GLfloat*)NULL + (i))

typedef struct CVertex
{
	GLfloat pos[3];
	GLfloat col[3];
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
    if( ! prog.compileShaderFromFile("shaders/basic.vert", shader::VERTEX))
    {
		printf("Vertex shader failed to compile!\n%s", prog.log().c_str());
		exit(1);
	}

    // Compile fragment shader
    if( ! prog.compileShaderFromFile("shaders/basic.frag", shader::FRAGMENT))
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
    
    // Our data is interleaved, allowing for both position and color information
    // to be combined into one array. Nifty for organization and performance
    CVertex packedData[] = {
    		{{ 0.8f, -0.8f, 0.0f},
    		 { 0.0f,  1.0f, 0.0f}},

    		{{-0.8f, -0.8f, 0.0f},
    		 { 1.0f,  0.0f, 0.0f}},

    		{{ 0.8f,  0.8f, 0.0f},
    		 { 0.0f,  0.0f, 1.0f}},

    		{{-0.8f,  0.8f, 0.0f},
    		 { 1.0f,  1.0f, 0.0f}}
    };

    // Build our vertex array object
    Vao vao;

    vao.setShaderProgram(prog.getHandle());
    vao.create(GL_ARRAY_BUFFER, 4 * sizeof(CVertex), packedData, GL_STATIC_DRAW);

    // We have a stride the size of our datastructure.  Also, BUFFER_OFFSET is
    // a simple macro to help us build an offset within the datastructure
    vao.bindAttribute("VertexPosition", 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0));
    vao.bindAttribute("VertexColor", 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3));

    do
    {
        glfwGetMousePos( &x, NULL );

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        // Draw our vertex array object
        vao.draw(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
