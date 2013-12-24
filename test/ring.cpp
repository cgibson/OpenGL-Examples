//========================================================================
// This example application builds off of strip.cpp and constructs
// a procedural ring out of a strip of quads
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <math.h>
#include "GLSLProgram.hpp"
#include "glUtil.hpp"
#include <vector>

using std::vector;

#define BUFFER_OFFSET(i) ((GLfloat*)NULL + (i))

using glm::mat4;
using glm::vec3;

typedef struct CVertex
{
    vec3 pos;
    vec3 col;
} CVertex;

int main( void )
{
    int width, height;
    double t;

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
    if( ! prog.compileShaderFromFile("shaders/basicview.vert", shader::VERTEX))
    {
        printf("Vertex shader failed to compile!\n%s", prog.log().c_str());
        exit(1);
    }

    // Compile fragment shader
    if( ! prog.compileShaderFromFile("shaders/basicview.frag", shader::FRAGMENT))
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
    GLint cLoc   = prog.getAttribLocation("VertexColor");
    
    printf("PLOC: %d\n", (int)pLoc);
    printf("CLOC: %d\n", (int)cLoc);

    GLuint vaoHandle;

    // Using a vector instead of a flat array
    vector<CVertex> packedData;

    // Draw a short ring.  We want to use a triangle strip, so organize the vertices accordingly
    float ringRad = 1.0f;
    float ringWidth = 0.25f;

    // We want the loop to touch both 0 and 360, in order to ensure a perfect loop
    for (int i = 0; i <= 360; i+=36) {

        // Convert to radians
        float r = (i / 360.0f) * 2 * M_PI;
        float x = cos(r) * ringRad;
        float z = sin(r) * ringRad;

        // Add upper/lower vertices
        packedData.push_back((CVertex){ vec3(x, -0.5 * ringWidth, z), vec3( 0.0f,  0.0f, 1.0f) });
        packedData.push_back((CVertex){ vec3(x,  0.5 * ringWidth, z), vec3( 1.0f,  0.0f, 0.0f) });
    }

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    GLuint dataBufferHandle = vboHandle;;

    // Populate position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);

    // We need to use the front() function in order to get the array pointer
    glBufferData(GL_ARRAY_BUFFER, packedData.size() * sizeof(CVertex), &packedData.front(),
            GL_STATIC_DRAW);


    // Create and set-up array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(pLoc); // Vertex position
    glEnableVertexAttribArray(cLoc); // Vertex color

    // Map index 0 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);

    glVertexAttribPointer( pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0) );
    glVertexAttribPointer( cLoc, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3) );


    vec3 eye(0,2,-3);
    vec3 lookAt(0);

    glEnable(GL_DEPTH_TEST);
    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        t = glfwGetTime();
        float r = (GLfloat)t*50.0f;

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        // Start using our shader
        prog.use();

        // We'll need all of these matrices
        mat4 proj, view, modelview, modelviewProj;

        // Set up the projection matrix
        proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

        // Use the lookAt function to build a view matrix
        view = glm::lookAt(eye, lookAt, vec3(0,1,0));

        // Create the model matrix, which we're using to rotate the model
        modelview = glm::rotate(glm::mat4(1.0f), r, vec3(0,1,0));

        // Finally, bring everything together for the modelViewProjection matrix
        modelviewProj = (proj * view * modelview);

        // Pass on the MVP matrix onto the program
        prog.setUniform("MVP", modelviewProj);

        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, packedData.size());

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
