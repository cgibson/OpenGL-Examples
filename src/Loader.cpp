#include "Loader.hpp"
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

        vector<uint> vertexIndices;
        vector<uint> normalIndices;
        vector<uint> uvIndices;

        bool hasUvs = true;
        bool hasNormals = true;
        int type = 0; // 0 == EMPTY, 1 == QUADS, 2 == TRIANGLES

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

                if (type) {
                    if (type != 1)
                        printf("Error: Model has both quads and triangles.\n");
                } else {
                    type = 1; // QUADS
                }

                if(iv1 < 1 || iv1 >= vertices.size() ||
                   iv2 < 1 || iv2 >= vertices.size() ||
                   iv3 < 1 || iv3 >= vertices.size() ||
                   iv4 < 1 || iv4 >= vertices.size()) {
                    printf("Error: vertex index out of bounds:\n\t[%s]\n", s.c_str());
                    continue;
                }
                if(in1 < 1 || in1 >= normals.size() ||
                   in2 < 1 || in2 >= normals.size() ||
                   in3 < 1 || in3 >= normals.size() ||
                   in4 < 1 || in4 >= normals.size()) {
                    printf("Error: normal index out of bounds:\n\t[%s]\n", s.c_str());
                    continue;
                }
                if(it1 < 1 || it1 >= uvs.size() ||
                   it2 < 1 || it2 >= uvs.size() ||
                   it3 < 1 || it3 >= uvs.size() ||
                   it4 < 1 || it4 >= uvs.size()) {
                    printf("Error: uv index out of bounds:\n\t[%s]\n", s.c_str());
                    continue;
                }

                vertexIndices.push_back(iv1);
                vertexIndices.push_back(iv2);
                vertexIndices.push_back(iv3);
                vertexIndices.push_back(iv4);

                normalIndices.push_back(in1);
                normalIndices.push_back(in2);
                normalIndices.push_back(in3);
                normalIndices.push_back(in4);

                uvIndices.push_back(it1);
                uvIndices.push_back(it2);
                uvIndices.push_back(it3);
                uvIndices.push_back(it4);

                continue;
            }

            // Face
            if (sscanf(s.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &iv1, &it1, &in1,
                       &iv2, &it2, &in2,
                       &iv3, &it3, &in3) == 9) {

                if (type) {
                    if (type != 2)
                        printf("Error: Model has both quads and triangles.\n");
                } else {
                    type = 2; // TRIANGLES
                }

                if(iv1 < 1 || iv1 > vertices.size() ||
                   iv2 < 1 || iv2 > vertices.size() ||
                   iv3 < 1 || iv3 > vertices.size()) {
                    printf("Error: vertex index out of bounds:\n\t[%s]\n", s.c_str());
                }
                if(in1 < 1 || in1 > normals.size() ||
                   in2 < 1 || in2 > normals.size() ||
                   in3 < 1 || in3 > normals.size()) {
                    printf("Error: normal index out of bounds:\n\t[%s]\n", s.c_str());
                }
                if(it1 < 1 || it1 > uvs.size() ||
                   it2 < 1 || it2 > uvs.size() ||
                   it3 < 1 || it3 > uvs.size()) {
                    printf("Error: uv index out of bounds:\n\t[%s]\n", s.c_str());
                }

                vertexIndices.push_back(iv1);
                vertexIndices.push_back(iv2);
                vertexIndices.push_back(iv3);

                normalIndices.push_back(in1);
                normalIndices.push_back(in2);
                normalIndices.push_back(in3);

                uvIndices.push_back(it1);
                uvIndices.push_back(it2);
                uvIndices.push_back(it3);

                //printf("ADDED TRI\n");
                continue;
            }

            // Face (no UVs)
            if (sscanf(s.c_str(), "f %d//%d %d//%d %d//%d",
                       &iv1, &in1,
                       &iv2, &in2,
                       &iv3, &in3) == 6) {

                if (type) {
                    if (type != 2)
                        printf("Error: Model has both quads and triangles.\n");
                } else {
                    type = 2; // TRIANGLES
                }

                if(iv1 < 1 || iv1 > vertices.size() ||
                   iv2 < 1 || iv2 > vertices.size() ||
                   iv3 < 1 || iv3 > vertices.size()) {
                    printf("Error: vertex index out of bounds:\n\t[%s]\n", s.c_str());
                    printf("\tMax size: %u\n", (uint)vertices.size());
                }
                if(in1 < 1 || in1 > normals.size() ||
                   in2 < 1 || in2 > normals.size() ||
                   in3 < 1 || in3 > normals.size()) {
                    printf("Error: normal index out of bounds:\n\t[%s]\n", s.c_str());
                    printf("\tMax size: %u\n", (uint)normals.size());
                }

                vertexIndices.push_back(iv1);
                vertexIndices.push_back(iv2);
                vertexIndices.push_back(iv3);

                normalIndices.push_back(in1);
                normalIndices.push_back(in2);
                normalIndices.push_back(in3);

                hasUvs = false;

                continue;
            }

            printf("Ignoring line:\n\t[%s]\n", s.c_str());
        }

        uint vertexIndex, uvIndex, normalIndex;
        for (uint i = 0; i < vertexIndices.size(); ++i)
        {
            vertexIndex = vertexIndices[i];
            m.vertices.push_back(vertices[vertexIndex - 1]);

            if(hasNormals) {
                normalIndex = normalIndices[i];
                m.normals.push_back(normals[normalIndex - 1]);
            }
            if(hasUvs) {
                uvIndex = uvIndices[i];
                m.uvs.push_back(uvs[uvIndex - 1]);
            }
        }

        return m;
    }
}
