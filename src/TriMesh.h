#ifndef TRIMESH_H
#define TRIMESH_H

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include "GL/glfw.h"

using std::vector;

using glm::vec4;
using glm::vec3;
using glm::vec2;

namespace mesh {

    class TriVert
    {
    public:
        TriVert(){}
        TriVert(vec3 const & v):x(v.x), y(v.y), z(v.z){}
        float x, y, z;
    };

    class UvVert
    {
    public:
        UvVert(){}
        UvVert(vec2 const & uv):u(uv.x), v(uv.y){}
        float u, v;
    };

    class TriMesh {
    public:
        vector<vec3> vertices;
        vector<vec3> normals;
        vector<vec2> uvs;
    private:
    };

    class CompressedTriMesh {
    public:
        CompressedTriMesh(TriMesh const & mesh, GLint renderType);
        ~CompressedTriMesh();
        void draw();
        int size() { return _count; }
        bool initialized() { return _initialized; }
    protected:
        int _count;
        float * _vertices;
        float * _normals;
        float * _uvs;
        bool _initialized;
        GLint _renderType;
    };

}

#endif //LOADER_H
