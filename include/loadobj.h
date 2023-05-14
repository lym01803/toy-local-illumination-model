#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <cstddef>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using std::vector;


class ModelObject {
public:
    vector< glm::vec3 > vertices;
    vector< glm::vec2 > uvs;
    vector< glm::vec3 > normals;
    bool status;
    ModelObject(){ };
    ModelObject(const char* loadpath);
    void apply(std::function<glm::vec3(glm::vec3)>);
    void append(ModelObject);
};

#endif