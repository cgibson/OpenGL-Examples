#include "Loader.h"
#include <iostream>
#include <fstream>
#include <vector>

using std::vector;

namespace mesh {
    TriMesh
    loadObj(string filename)
    {
        TriMesh m;

        std::ifstream fin(filename.c_str());

        float x, y, z;
        unsigned int iv1, it1, in1;
        unsigned int iv2, it2, in2;
        unsigned int iv3, it3, in3;
        unsigned int iv4, it4, in4;

        vector<vec3> vertices;
        vector<vec2> uvs;
        vector<vec3> normals;

        string s;
        while (getline( fin, s )) {

            // Vertex
            if (sscanf(s.c_str(), "v %f %f %f", &x, &y, &z) == 3) {
                vertices.push_back(vec3(x,y,z));
                continue;
            }

            // Vertex UV
            if (sscanf(s.c_str(), "vt %f %f", &x, &y) == 2) {
                uvs.push_back(vec2(x,y));
                printf("TEXTURE: %f %f\n", x, y);
                continue;
            }

            // Vertex normal
            if (sscanf(s.c_str(), "vn %f %f %f", &x, &y, &z) == 3) {
                normals.push_back(vec3(x,y,z));
                continue;
            }

            // Face (Quad)
            if (sscanf(s.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                       &iv1, &it1, &in1,
                       &iv2, &it2, &in2,
                       &iv3, &it3, &in3,
                       &iv4, &it4, &in4) == 12) {

                // Set to zero-based indexes
                iv1--; iv2--; iv3--; iv4--;
                it1--; it2--; it3--; it4--;
                in1--; in2--; in3--; in4--;

                if(iv1 < 0 || iv1 >= vertices.size() ||
                   iv2 < 0 || iv2 >= vertices.size() ||
                   iv3 < 0 || iv3 >= vertices.size() ||
                   iv4 < 0 || iv4 >= vertices.size()) {
                    printf("Error: vertex index out of bounds:\n\t[%s]\n", s.c_str());
                    continue;
                }
                if(in1 < 0 || in1 >= normals.size() ||
                   in2 < 0 || in2 >= normals.size() ||
                   in3 < 0 || in3 >= normals.size() ||
                   in4 < 0 || in4 >= normals.size()) {
                    printf("Error: normal index out of bounds:\n\t[%s]\n", s.c_str());
                    continue;
                }
                if(it1 < 0 || it1 >= uvs.size() ||
                   it2 < 0 || it2 >= uvs.size() ||
                   it3 < 0 || it3 >= uvs.size() ||
                   it4 < 0 || it4 >= uvs.size()) {
                    printf("Error: uv index out of bounds:\n\t[%s]\n", s.c_str());
                    continue;
                }

                m.vertices.push_back(vertices[iv1]);
                m.vertices.push_back(vertices[iv2]);
                m.vertices.push_back(vertices[iv3]);
                m.vertices.push_back(vertices[iv1]);
                m.vertices.push_back(vertices[iv3]);
                m.vertices.push_back(vertices[iv4]);


                m.normals.push_back(normals[in1]);
                m.normals.push_back(normals[in2]);
                m.normals.push_back(normals[in3]);
                m.normals.push_back(normals[in1]);
                m.normals.push_back(normals[in3]);
                m.normals.push_back(normals[in4]);

                m.uvs.push_back(uvs[it1]);
                m.uvs.push_back(uvs[it2]);
                m.uvs.push_back(uvs[it3]);
                m.uvs.push_back(uvs[it1]);
                m.uvs.push_back(uvs[it3]);
                m.uvs.push_back(uvs[it4]);

                continue;
            }

            // Face
            if (sscanf(s.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &iv1, &it1, &in1,
                       &iv2, &it2, &in2,
                       &iv3, &it3, &in3) == 9) {

                // Set to zero-based indexes
                iv1--; iv2--; iv3--;
                it1--; it2--; it3--;
                in1--; in2--; in3--;

                if(iv1 < 0 || iv1 > vertices.size() ||
                   iv2 < 0 || iv2 > vertices.size() ||
                   iv3 < 0 || iv3 > vertices.size()) {
                    printf("Error: vertex index out of bounds:\n\t[%s]\n", s.c_str());
                }
                if(in1 < 0 || in1 > normals.size() ||
                   in2 < 0 || in2 > normals.size() ||
                   in3 < 0 || in3 > normals.size()) {
                    printf("Error: normal index out of bounds:\n\t[%s]\n", s.c_str());
                }
                if(it1 < 0 || it1 > uvs.size() ||
                   it2 < 0 || it2 > uvs.size() ||
                   it3 < 0 || it3 > uvs.size()) {
                    printf("Error: uv index out of bounds:\n\t[%s]\n", s.c_str());
                }

                m.vertices.push_back(vertices[iv1]);
                m.vertices.push_back(vertices[iv2]);
                m.vertices.push_back(vertices[iv3]);

                m.normals.push_back(normals[in1]);
                m.normals.push_back(normals[in2]);
                m.normals.push_back(normals[in3]);

                m.uvs.push_back(uvs[it1]);
                m.uvs.push_back(uvs[it2]);
                m.uvs.push_back(uvs[it3]);

                printf("ADDED TRI\n");
                continue;
            }

            printf("Ignoring line:\n\t[%s]\n", s.c_str());
        }

        return m;
    }
}
