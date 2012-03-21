#include "TriMesh.h"

#define VERT2_INDEX(tri,vert) tri*2+vert
#define VERT3_INDEX(tri,vert) tri*3+vert
#define VERT4_INDEX(tri,vert) tri*4+vert

enum {
	ATTRIB_VERTEX,
	ATTRIB_NORMAL,
    ATTRIB_UV,
    ATTRIB_COLOR,
	NUM_ATTRIBUTES
};

namespace mesh {

    CompressedTriMesh::CompressedTriMesh(TriMesh const & m, GLint renderType):
        _renderType(renderType)
    {
        assert(m.vertices.size() == m.uvs.size());
        assert(m.vertices.size() == m.normals.size());

        _count = m.vertices.size();
        _vertices = new float[_count * 3];
        _uvs = new float[_count * 3];
        _normals = new float[_count * 3];

        printf("Count: %d\n", _count);

        for( int i=0; i<_count; i++ ) {
            vec3 vert = m.vertices[i];
            vec3 normal = m.normals[i];
            vec2 uv = m.uvs[i];
            _vertices[VERT3_INDEX(i, 0)] = vert.x;
            _vertices[VERT3_INDEX(i, 1)] = vert.y;
            _vertices[VERT3_INDEX(i, 2)] = vert.z;

            _normals[VERT3_INDEX(i, 0)] = normal.x;
            _normals[VERT3_INDEX(i, 1)] = normal.y;
            _normals[VERT3_INDEX(i, 2)] = normal.z;

            _uvs[VERT2_INDEX(i, 0)] = uv.x;
            _uvs[VERT2_INDEX(i, 1)] = uv.y;

        }
        _initialized = true;
    }

    CompressedTriMesh::~CompressedTriMesh()
    {
        delete _vertices;
        delete _normals;
        delete _uvs;
    }

    void
    CompressedTriMesh::draw()
    {
        if (!_initialized) return;

        glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, _vertices);
        glEnableVertexAttribArray(ATTRIB_VERTEX);

        glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, 0, 0, _normals);
        glEnableVertexAttribArray(ATTRIB_NORMAL);

        glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, 0, 0, _uvs);
        glEnableVertexAttribArray(ATTRIB_UV);

        glDrawArrays(_renderType, 0, _count);

    }

}
