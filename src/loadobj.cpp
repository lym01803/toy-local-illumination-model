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
    status = true;
}