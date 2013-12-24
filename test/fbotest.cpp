//========================================================================
// This test application exemplifies the construction
// and use of Frame Buffer Objects (FBOs) which can be
// used to render onto off-screen buffers.
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgram.hpp"
#include "glUtil.hpp"
#include "vao.hpp"
#include "fbo.hpp"

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
    int width, height, x;
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

    // COMPILE FBO SHADER
    ////////////////////////////////////////////////////////////////////////////
    shader::GLSLProgram fboprog;

    // Compile vertex shader
    if( ! fboprog.compileShaderFromFile("shaders/fbo.vert", shader::VERTEX))
    {
        printf("Vertex shader failed to compile!\n%s", fboprog.log().c_str());
        exit(1);
    }

    // Compile fragment shader
    if( ! fboprog.compileShaderFromFile("shaders/fbo.frag", shader::FRAGMENT))
    {
        printf("Fragment shader failed to compile!\n%s", fboprog.log().c_str());
        exit(1);
    }

    // Link shaders
    if( ! fboprog.link() )
    {
        printf("Shader program failed to link!\n%s", fboprog.log().c_str());
        exit(1);
    }

    // END COMPILE FBO SHADER
    ////////////////////////////////////////////////////////////////////////////

    // Build the object to render within the fbo
    vector<CVertex> packedData;
    packedData.push_back((CVertex){vec3( 0.8f, -0.8f, 0.0f), vec3( 0.0f,  1.0f, 0.0f)});
    packedData.push_back((CVertex){vec3(-0.8f, -0.8f, 0.0f), vec3( 1.0f,  0.0f, 0.0f)});
    packedData.push_back((CVertex){vec3( 0.8f,  0.8f, 0.0f), vec3( 0.0f,  0.0f, 1.0f)});
    packedData.push_back((CVertex){vec3(-0.8f,  0.8f, 0.0f), vec3( 1.0f,  1.0f, 0.0f)});

    Vao vao;
    vao.create(GL_ARRAY_BUFFER, 4 * sizeof(CVertex), &packedData.front(), GL_STATIC_DRAW);
    vao.setShaderProgram(prog.getHandle());
    vao.bindAttribute("VertexPosition", 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0));
    vao.bindAttribute("VertexColor", 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3));

    // Build the FBO plane
    vector<CVertex> fboPlaneData;
    fboPlaneData.push_back((CVertex){vec3( 1.0f, -1.0f, 0.0f), vec3( 1.0f, 0.0f, 0.0f )});
    fboPlaneData.push_back((CVertex){vec3(-1.0f, -1.0f, 0.0f), vec3( 0.0f, 0.0f, 0.0f )});
    fboPlaneData.push_back((CVertex){vec3( 1.0f,  1.0f, 0.0f), vec3( 1.0f, 1.0f, 0.0f )});
    fboPlaneData.push_back((CVertex){vec3(-1.0f,  1.0f, 0.0f), vec3( 0.0f, 1.0f, 0.0f )});

    Vao fboVao;
    fboVao.create(GL_ARRAY_BUFFER, 4 * sizeof(CVertex), &fboPlaneData.front(), GL_STATIC_DRAW);
    fboVao.setShaderProgram(fboprog.getHandle());
    fboVao.bindAttribute("VertexPosition", 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0));
    fboVao.bindAttribute("VertexUV", 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3));

    Fbo fbo;
    int r = fbo.create(640, 480, 1);

    if (!r) {
        printf("SOMETHING WENT WRONG!\n");
    }

    vec3 eye(0,0,-3);
    vec3 lookAt(0);

    do
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        t = glfwGetTime();
        float r = 0.3f*(GLfloat)x + (GLfloat)t*100.0f;
        glfwGetMousePos( &x, NULL );

        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        // RENDER INTO FBO
        ////////////////////////////////////////////////////////////////////////
        fbo.enable();
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        prog.use();

        mat4 proj, view, modelview, modelviewProj, normal;
        proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
        view = glm::lookAt(eye, lookAt, vec3(0,1,0));
        modelview = glm::rotate(glm::mat4(1.0f), r, vec3(0.5,1.2,0.3));
        modelviewProj = (proj * view * modelview);

        normal = glm::inverse(view * modelview);
        normal = glm::transpose(normal);

        prog.setUniform("MVP", modelviewProj);

        vao.draw(GL_TRIANGLE_STRIP, 0, 4);

        fbo.disable();

        // RENDER FBO TO SCREEN
        ////////////////////////////////////////////////////////////////////////

        // Set the FBO texture to texture zero
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo.getTextureHandles()[0]);

        // We could create an orthographic view, but that'd be boring!
        //mat4 modelviewProj = glm::ortho(0.0f, 0.0f, 640.0f, 480.0f, -1.0f, 1.0f);

        // Instead, let's spin the quad!
        proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
        view = glm::lookAt(eye, lookAt, vec3(0,1,0));
        modelview = glm::rotate(glm::mat4(1.0f), r * 0.4f, vec3(0,1,0));
        modelviewProj = (proj * view * modelview);

        // Begin using the FBO shader
        fboprog.use();
        //fboprog.setUniform("ViewMatrix", view2);
        fboprog.setUniform("FboTexture", 0);
        fboprog.setUniform("MVP", modelviewProj);

        fboVao.draw(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
