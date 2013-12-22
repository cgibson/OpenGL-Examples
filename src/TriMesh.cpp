#include "TriMesh.hpp"

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

    void
    TriMesh::normalize(float radius)
    {
        float min_x, min_y, min_z;
        float max_x, max_y, max_z;
        vec3 cur = this->vertices[0];

        min_x = cur.x;
        min_y = cur.y;
        min_z = cur.z;
        max_x = cur.x;
        max_y = cur.y;
        max_z = cur.z;

        for (unsigned int i = 1; i < this->vertices.size(); i++)
        {
            cur = this->vertices[i];
            if (cur.x < min_x)
                min_x = cur.x;
            if (cur.y < min_y)
                min_y = cur.y;
            if (cur.z < min_z)
                min_z = cur.z;

            if (cur.x > max_x)
                max_x = cur.x;
            if (cur.y > max_y)
                max_y = cur.y;
            if (cur.z > max_z)
                max_z = cur.z;
        }

        vec3 size((max_x - min_x),
                  (max_y - min_y),
                  (max_z - min_z));

        float size_max;
        // Find out the biggest
        if (size.x > size.y) {
            if (size.x > size.z) {
                size_max = size.x;
            }else{
                size_max = size.z;
            }
        } else{
            if (size.y > size.z) {
                size_max = size.y;
            }else{
                size_max = size.z;
            }
        }

        vec3 min(min_x, min_y, min_z);
        vec3 max(max_x, max_y, max_z);

        vec3 center = (max + min);
        center *= 0.5;

        float scale = radius / size_max;

        printf("CENTER: %.3f %.3f %.3f\n", center.x, center.y, center.z);

        for (unsigned int i = 0; i < this->vertices.size(); i++)
        {
            cur = this->vertices[i];

            cur -= center;
            cur *= scale;

            this->vertices[i] = cur;

        }
    }

    CompressedTriMesh::CompressedTriMesh(TriMesh const & m, GLint renderType):
        _renderType(renderType)
    {
        assert(m.vertices.size() == m.uvs.size());
        assert(m.vertices.size() == m.normals.size());

        _count = m.vertices.size();
        _vertices = new float[_count * 3];
        _uvs = new float[_count * 3];
        _normals = new float[_count * 3];

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
