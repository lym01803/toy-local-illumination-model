#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/fwd.hpp"
#include "loadshader.h"
#include <cstring>
#include <loadobj.h>


ModelObject::ModelObject(const char* loadpath) {
    status = false;
    FILE *file = fopen(loadpath, "r");
    if (file == NULL) {
        printf("Error occurred when loading %s\n", loadpath);
        return;
    }
    vector< glm::vec3 > temp_vertices;
    vector< glm::vec2 > temp_uvs;
    vector< glm::vec3 > temp_normals;
    while (1) {
        char line[256];
        int res = fscanf(file, "%s", line);
        if (res == EOF) {
            break;
        }
        if (strcmp(line, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if(strcmp(line, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if(strcmp(line, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if(strcmp(line, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", 
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            vertices.push_back(temp_vertices[vertexIndex[0] - 1]);
            vertices.push_back(temp_vertices[vertexIndex[1] - 1]);
            vertices.push_back(temp_vertices[vertexIndex[2] - 1]);
            uvs.push_back(temp_uvs[uvIndex[0] - 1]);
            uvs.push_back(temp_uvs[uvIndex[1] - 1]);
            uvs.push_back(temp_uvs[uvIndex[2] - 1]);
            normals.push_back(temp_normals[normalIndex[0] - 1]);
            normals.push_back(temp_normals[normalIndex[1] - 1]);
            normals.push_back(temp_normals[normalIndex[2] - 1]);
        }
    }
    fclose(file);
    status = true;
}

void ModelObject::apply(std::function<glm::vec3(glm::vec3)> f) {
    for (int i = 0; i < vertices.size(); ++i) {
        vertices[i] = f(vertices[i]);
    }
    // for (int i = 0;i < normals.size(); ++i) {
    //     normals[i] = glm::normalize(f(normals[i]));
    // }
}

void ModelObject::append(ModelObject otherobj) {
    vertices.insert(vertices.end(), otherobj.vertices.begin(), otherobj.vertices.end());
    uvs.insert(uvs.end(), otherobj.uvs.begin(), otherobj.uvs.end());
    normals.insert(normals.end(), otherobj.normals.begin(), otherobj.normals.end());
    // for (int i = 0; i < otherobj.vertices.size(); ++i) { 
    //     vertices.push_back(otherobj.vertices[i]);
    // }
    // for (int i = 0; i < otherobj.uvs.size(); ++i) {
    //     uvs.push_back(otherobj.uvs[i]);
    // }
    // for (int i = 0; i < otherobj.normals.size(); ++i) {
    //     normals.push_back(otherobj.normals[i]);
    // }
}

void ModelObject::multiby(glm::mat3 matrix) {
    for (auto iter = vertices.begin(); iter != vertices.end(); ++iter) {
        *iter = matrix * (*iter);
    }
    for (auto iter = normals.begin(); iter != normals.end(); ++iter) {
        *iter = glm::normalize(matrix * (*iter));
    }
}

void ModelObject::multiby(glm::mat4 matrix) {
    glm::vec4 temp;
    for (auto iter = vertices.begin(); iter != vertices.end(); ++iter) {
        temp = glm::vec4(*iter, 1.0);
        temp = matrix * temp;
        *iter = glm::vec3(temp.x, temp.y, temp.z);
    }
    for (auto iter = normals.begin(); iter != normals.end(); ++iter) {
        temp = glm::vec4(*iter, 0.0);
        temp = glm::normalize(matrix * temp);
        *iter = glm::vec3(temp.x, temp.y, temp.z);
    }
}

void loadObjectsfromTxt(const char* path, vector<ModelObject> & objlist) {
    FILE * file = fopen(path, "r");
    if (file == NULL) {
        printf("Error occurred when loading %s\n", path);
        return;
    }
    float mat[4][4];
    while (1) {
        char line[256];
        int res = fscanf(file, "%s", line);
        if (res == EOF) {
            break;
        }
        // printf("debug-0: %s\n", line);
        if (strcmp(line, "#") == 0) {
            fscanf(file, "%s", line);
            // printf("debug-1: %s\n", line);
            ModelObject obj = ModelObject(line);
            if (!obj.status) {
                printf("Error occurred when loading %s\n", line);
            } 
            while (1) {
                res = fscanf(file, "%s", line);
                // printf("debug-2: %s\n", line);
                if (res == EOF) {
                    printf("Error occurred when loading %s\n", path);
                    return;
                }
                if (strcmp(line, "end") == 0) {
                    break;
                }
                else if (strcmp(line, "operate") == 0) {
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            fscanf(file, "%f", &mat[i][j]);
                        }
                    }
                    obj.multiby(glm::mat4(
                        mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                        mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                        mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                        mat[0][3], mat[1][3], mat[2][3], mat[3][3]
                    ));
                }
            }
            objlist.push_back(obj);
        }
    }
    fclose(file);
}
