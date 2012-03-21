#ifndef LOADER_H
#define LOADER_H

#include "TriMesh.h"
#include <string>

using std::string;

namespace mesh {

    TriMesh loadObj(string filename);

}

#endif //LOADER_H
