#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <glm/common.hpp>
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

class ObjLoader
{
public:
    ObjLoader();
    ~ObjLoader();

    static bool LoadObj(
        const char * path,
        std::vector < glm::vec3 > & out_vertices,
        std::vector < glm::vec2 > & out_uvs,
        std::vector < glm::vec3 > & out_normals
    );
};
