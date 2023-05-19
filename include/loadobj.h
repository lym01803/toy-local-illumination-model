#ifndef LOADOBJ_H
#define LOADOBJ_H

#include "glm/ext/vector_float4.hpp"
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
    void multiby(glm::mat3);
    void multiby(glm::mat4);
    void append(ModelObject);
};

void loadObjectsfromTxt(const char* path, vector<ModelObject> & objlist);

#endif