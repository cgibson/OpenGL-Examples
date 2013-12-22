//========================================================================
// Here we rely on Loader.cpp and TriMesh.cpp to load the contents
// of a 3d .obj file into a VAO. For simpler Vertex Array Object
// examples, check quad.cpp or ring.cpp
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

#include "Loader.hpp"
#include "TriMesh.hpp"

#define BUFFER_OFFSET(i) ((GLfloat*)NULL + (i))

using glm::mat4;
using glm::vec3;
using std::vector;

typedef struct CVertex
{
    vec3 pos;
    vec3 normal;
} CVertex;

int main( int argc, char* argv[] )
{
    int width, height;
    double t;

    string modelPath = "models/bunny2.obj";
    if (argc == 2)
        modelPath = string(argv[1]);

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
    if( !glfwOpenWindow( 640, 480, 0,0,0,0, 0,0, GLFW_WINDOW ) )
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

    glfwSetWindowTitle( "Mesh Viewer" );

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );

    shader::GLSLProgram prog;

    // Compile vertex shader
    if( ! prog.compileShaderFromFile("shaders/basicshade.vert", shader::VERTEX))
    {
        printf("Vertex shader failed to compile!\n%s", prog.log().c_str());
        exit(1);
    }

    // Compile fragment shader
    if( ! prog.compileShaderFromFile("shaders/basicshade.frag", shader::FRAGMENT))
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
    GLint nLoc   = prog.getAttribLocation("VertexNormal");

    printf("Vertex Position Attrib Loc: %d\n", (int)pLoc);
    printf("Vertex Normal Attrib Loc: %d\n", (int)nLoc);

    GLuint vaoHandle;

    // Using a vector instead of a flat array
    vector<CVertex> packedData;

    // Load the model into a TriMesh object
    mesh::TriMesh mesh = mesh::loadObj(modelPath);
    mesh.normalize(2);

    // Load the data from our TriMesh into a packed format
    for (uint i = 0; i < mesh.vertices.size(); i++) {
        packedData.push_back((CVertex) { mesh.vertices[i], mesh.normals[i] });
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
    glEnableVertexAttribArray(nLoc); // Vertex normal

    // Map index 0 to the position buffer
    glBindBuffer(GL_ARRAY_BUFFER, dataBufferHandle);

    glVertexAttribPointer( pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(0) );
    glVertexAttribPointer( nLoc, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), BUFFER_OFFSET(3) );


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
        mat3 normal;

        // Set up the projection matrix
        proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

        // Use the lookAt function to build a view matrix
        view = glm::lookAt(eye, lookAt, vec3(0,1,0));

        // Create the model matrix, which we're using to rotate the model
        modelview = glm::rotate(glm::mat4(1.0f), r, vec3(0,1,0));

        // Finally, bring everything together for the modelViewProjection matrix
        modelviewProj = (proj * view * modelview);

        // Create our normal matrix, which is just an inverse transpose of the modelview matrix
        normal = glm::mat3(glm::inverse(view * modelview));
        normal = glm::transpose(normal);

        // Pass on the MVP and normal matrix onto the program
        prog.setUniform("MVP", modelviewProj);
        prog.setUniform("NormalMatrix", normal);

        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_TRIANGLES, 0, packedData.size());

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
