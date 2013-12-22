//========================================================================
// This is a small test application for which implements a
// simple camera in OpenGL. The camera look direction is
// controlled by the mouse, and keys move the camera location
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

#define PI 3.1415

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

    GLuint vaoHandle;

    // Using a vector instead of a flat array
    vector<CVertex> packedData;

    packedData.push_back((CVertex){ vec3(-0.5, -0.5, 0), vec3( 0.0f,  0.0f, 1.0f) });
    packedData.push_back((CVertex){ vec3(-0.5,  0.5, 0), vec3( 1.0f,  0.0f, 0.0f) });
    packedData.push_back((CVertex){ vec3( 0.5,  0.5, 0), vec3( 0.0f,  1.0f, 1.0f) });
    packedData.push_back((CVertex){ vec3( 0.5, -0.5, 0), vec3( 0.0f,  1.0f, 0.0f) });

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


    // A bunch of variable declarations, including several for camera location, up vector and look dir
    vec3 eye(0,0,-3);
    vec3 lookDir(0,0,1);
    vec3 up(0,1,0);
    vec3 up_normal(0,1,0);
    vec3 up_reverse(0,-1,0);
    vec3 lookDirRight = glm::cross(lookDir, up);
    vec3 lookDirUp = glm::cross(lookDirRight,lookDir);

    // Mouse status variables
    vec2 mouse_i, mouse;
    bool mouseDown = false;

    float yaw = 0.0, tilt = 0.0, move_speed = 3.0, rot_speed = 2.0;

    float t = glfwGetTime(), t_i, td;
    glEnable(GL_DEPTH_TEST);
    do
    {
        // Get window size (may be different than the requested size)
        glfwGetWindowSize( &width, &height );

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        t_i = t;
        t = glfwGetTime();
        td = t-t_i;

        // Key logic
        bool changedPos = false;
        vec3 move(0);
        if( glfwGetKey('W') == GLFW_PRESS ){
        	move += lookDir;
        	changedPos = true;
        }
        if( glfwGetKey('A') == GLFW_PRESS ){
        	move -= lookDirRight;
        	changedPos = true;
        }
        if( glfwGetKey('S') == GLFW_PRESS ){
        	move -= lookDir;
        	changedPos = true;
        }
        if( glfwGetKey('D') == GLFW_PRESS ){
        	move += lookDirRight;
        	changedPos = true;
        }
        if( glfwGetKey('R') == GLFW_PRESS ){
        	move += lookDirUp;
        	changedPos = true;
        }
        if( glfwGetKey('F') == GLFW_PRESS ){
        	move -= lookDirUp;
        	changedPos = true;
        }

        // Pretty straight forward. Normalize the sucker (only if we have moved) and add the
        // movement to our eye.
        if( changedPos ){
        	if( glm::length(move) > 0.1 ){
        		move = glm::normalize(move);
        	}
        	eye += move * td * move_speed;
        }

        // Mouse logic
        if( glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){

        	// Check to see if we already have the mouse down. If not, initialize the
        	// initial mouse location
        	if( !mouseDown ) {
            	int m_x, m_y;
        		glfwGetMousePos(&m_x, &m_y);
        		mouse_i = vec2(m_x, m_y);
        	}

        	// Yep, now we have the mouse pressed
        	mouseDown = true;

        	// Grab the current location of the mouse and find the delta between it and the last
        	int m_x, m_y;
        	glfwGetMousePos(&m_x, &m_y);
        	mouse = vec2(m_x, m_y);
        	vec2 mouse_d = mouse - mouse_i;
        	mouse_i = mouse;

        	// Find the distance the mouse traveled and multiply by the rotation speed
        	tilt -= (float)mouse_d.y / (float)height * rot_speed;
        	yaw -= (float)mouse_d.x / (float)width * rot_speed;

        	// Ensures that tilt stays within a certain range
        	while( tilt < (PI * -1) ){
        		tilt += (2 * PI);
        	}
        	while( tilt > PI ){
        		tilt -= (2 * PI);
        	}

        	// Ensures that the camera understands that "up" can mean "down" when the camera
        	// is upside down. Fixes some display and movement behavior
        	if( (tilt > (0.5*PI)) || (tilt < (-0.5*PI))){
        		up = up_reverse;
        	}else{
        		up = up_normal;
        	}

        	// Using tilt and role create a look direction. We need it to be a unit-vector value
        	lookDir = glm::normalize( vec3(sinf(yaw) * cos(tilt), sin(tilt), cosf(yaw) * cos(tilt)) );
        	// We need a strafe vector
        	lookDirRight = glm::cross(lookDir, up);
        	// Moving vertically respective to how/where you're looking just needs one more vector
        	lookDirUp = glm::cross(lookDirRight,lookDir);
        }else{
        	// We've either never touched the mouse or released it. Either's fine
        	mouseDown = false;
        }

        vec3 lookAt = eye + lookDir;

        // update rotation
        float r = 0;//(GLfloat)t*50.0f;

        // Special case: avoid division by zero below
        height = height > 0 ? height : 1;

        // Start using our shader
        prog.use();

        // We'll need all of these matrices
        mat4 proj, view, modelview, modelviewProj;

        // Set up the projection matrix
        proj = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

        // Use the lookAt function to build a view matrix
        view = glm::lookAt(eye, lookAt, up);

        // Create the model matrix, which we're using to rotate the model
        modelview = glm::rotate(glm::mat4(1.0f), r, up);

        // Finally, bring everything together for the modelViewProjection matrix
        modelviewProj = (proj * view * modelview);

        // Pass on the MVP matrix onto the program
        prog.setUniform("MVP", modelviewProj);

        glBindVertexArray(vaoHandle);
        glDrawArrays(GL_QUADS, 0, packedData.size());

        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    exit( EXIT_SUCCESS );
}
