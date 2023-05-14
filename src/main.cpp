#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <loadshader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <loadobj.h>

#define V_POSITION 0

GLuint LoadBMPTexture(const char * imagePath) {
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height, size;
    
    FILE * file = fopen(imagePath, "rb");
    if (file == NULL) {
        printf("Cannot open image file %s\n", imagePath);
        return 0;
    }
    if (fread(header, sizeof(unsigned char), 54, file) != 54) {
        printf("Cannot read bmp header\n");
        return 0;
    }
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct bmp file\n");
        return 0;
    }

    dataPos = *((int*)&header[0x0A]);
    size = *((int*)&header[0x22]);
    width = *((int*)&header[0x12]);
    height = *((int*)&header[0x16]);

    if (size == 0) {
        size = width * height * 3;
    }
    if (dataPos == 0) {
        dataPos = 54;
    }

    unsigned char * data = new unsigned char [size];
    fread(data, sizeof(unsigned char), size, file);
    fclose(file);

    GLuint TextureID;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    return TextureID;
}

ModelObject obj;

static GLfloat cube_vertex[8 * 3] = {
    1.000000, 1.000000, -1.000000,
    1.000000, -1.000000, -1.000000,
    1.000000, 1.000000, 1.000000,
    1.000000, -1.000000, 1.000000,
    -1.000000, 1.000000, -1.000000,
    -1.000000, -1.000000, -1.000000,
    -1.000000, 1.000000, 1.000000,
    -1.000000, -1.000000, 1.000000
};

static GLfloat cube_color[8 * 3] = {
    0., 0., 0.,
    0., 0., 1.,
    0., 1., 0.,
    0., 1., 1.,
    1., 0., 0.,
    1., 0., 1.,
    1., 1., 0.,
    1., 1., 1.
};

static GLfloat UV[14 * 2] = {
    0.628215, 1. - 0.497363,
    0.431948, 1. - 0.497363,
    0.628215, 1. - 0.747363,
    0.431948, 1. - 0.747363,
    0.824482, 1. - 0.497363,
    0.628215, 1. - 0.247363,
    0.235681, 1. - 0.497363,
    0.431948, 1. - 0.247363,
    0.824482, 1. - 0.747363,
    0.628215, 1. - 0.997363,
    0.628215, 1. - 0.002637,
    0.431948, 1. - 0.997363,
    0.431948, 1. - 0.002637,
    0.235681, 1. - 0.747363
};

void add_triangles_vertex(GLfloat * triangles, GLfloat * vertex, int idTri, int idA, int idB, int idC) {
    GLfloat * start = triangles + idTri * 9;
    memcpy(start + 0, vertex + idA * 3, sizeof(GLfloat) * 3);
    memcpy(start + 3, vertex + idB * 3, sizeof(GLfloat) * 3);
    memcpy(start + 6, vertex + idC * 3, sizeof(GLfloat) * 3);
}

void add_UV_vertex(GLfloat * UVArray, GLfloat * uv, int idTri, int idA, int idB, int idC) {
    GLfloat * start = UVArray + idTri * 6;
    memcpy(start + 0, uv + idA * 2, sizeof(GLfloat) * 2);
    memcpy(start + 2, uv + idB * 2, sizeof(GLfloat) * 2);
    memcpy(start + 4, uv + idC * 2, sizeof(GLfloat) * 2);
}

GLfloat triangles_data[3 * 3 * 12];
GLfloat triangles_color[3 * 3 * 12];
GLfloat triangles_UV[3 * 2 * 12];

GLuint VertexBuffer;
GLuint VertexArrayID;
GLuint VertexColorBufffer;
GLuint VertexNormBuffer;

glm::mat4 * GlobalMVP = NULL;
glm::mat4 * GlobalProjection = NULL;
glm::mat4 * GlobalView = NULL;
glm::mat4 * GlobalModel = NULL;

glm::vec3 * GlobalEye = NULL;

glm::mat3 RotateToXOY(glm::vec3 vec) {
    GLfloat r = sqrt((vec.x * vec.x + vec.z * vec.z));
    GLfloat c = vec.x / r;
    GLfloat s = vec.z / r;
    return glm::mat3(c, 0.f, -s, 0.f, 1.f, 0.f, s, 0.f, c);
}

GLfloat theta = acos(-1.) / 30;
glm::mat3 RotateStepXOY = glm::mat3(cos(theta), sin(theta), 0.f, -sin(theta), cos(theta), 0.f, 0.f, 0.f, 1.f);
glm::mat3 RotateStepXOZ = glm::mat3(cos(theta), 0.f, sin(theta), 0.f, 1.f, 0.f, -sin(theta), 0.f, cos(theta));

void error_callback(int error, const char * description){
    fprintf(stderr, "Error: %s\n", description);
}

void updateMVP() {
    *GlobalView = glm::mat4(glm::lookAt(
        *GlobalEye,
        glm::vec3(0., 0., 0.),
        glm::vec3(0., 1., 0.)
    ));
    *GlobalMVP = (*GlobalProjection) * (*GlobalView) * (*GlobalModel);
}
glm::mat3 Scale = glm::mat3(1.1f, 0.f, 0.f, 0.f, 1.1f, 0.f, 0.f, 0.f, 1.1f);

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_A && (action & (GLFW_PRESS | GLFW_REPEAT))) {
        if (GlobalEye) {
            *GlobalEye = RotateStepXOZ * (*GlobalEye);
            updateMVP();
        }
    }
    else if (key == GLFW_KEY_D && (action & (GLFW_PRESS | GLFW_REPEAT))) {
        if (GlobalEye) {
            *GlobalEye = glm::inverse(RotateStepXOZ) * (*GlobalEye);
            updateMVP();
        }
    }
    else if (key == GLFW_KEY_W && (action & (GLFW_PRESS | GLFW_REPEAT))) {
        if (GlobalEye) {
            glm::mat3 toXOY = RotateToXOY(*GlobalEye);
            // glm::vec3 copy = glm::vec3(*GlobalEye);
            *GlobalEye = glm::inverse(toXOY) * RotateStepXOY * toXOY * (*GlobalEye);
            // std::cout << glm::dot(*GlobalEye, copy) << std::endl;
            // if (glm::dot(*GlobalEye, copy) < 0) {
            //     *GlobalEye = -(*GlobalEye);
            // }
            updateMVP();
        }
    }
    else if (key == GLFW_KEY_S && (action & (GLFW_PRESS | GLFW_REPEAT))) {
        if (GlobalEye) {
            glm::mat3 toXOY = RotateToXOY(*GlobalEye);
            // glm::vec3 copy = glm::vec3(*GlobalEye);
            *GlobalEye = glm::inverse(toXOY) * glm::inverse(RotateStepXOY) * toXOY * (*GlobalEye);
            // std::cout << glm::dot(*GlobalEye, copy) << std::endl;
            // if (glm::dot(*GlobalEye, copy) < 0) {
            //     *GlobalEye = -(*GlobalEye);
            // }
            updateMVP();
        }
    }
    else if (key == GLFW_KEY_E && (action & (GLFW_PRESS | GLFW_REPEAT))) {
        if (GlobalEye) {
            *GlobalEye = Scale * (*GlobalEye);
            updateMVP();
        }
    }
    else if (key == GLFW_KEY_Q && (action & (GLFW_PRESS | GLFW_REPEAT))) {
        if (GlobalEye && GlobalEye->length() > 1.0f) {
            *GlobalEye = glm::inverse(Scale) * (*GlobalEye);
            updateMVP();
        }
    }
}

void init() {
    
    glGenBuffers(1, &VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    printf("Buffer Bound.\n");

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    printf("Vertex Array Bound.\n");

    static const GLfloat vertex_data[] = {
        -1.f, -1.f,
        1.f, -1.f, 
        0.f, 1.f,
    };

    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    // add_triangles_vertex(triangles_data, cube_vertex, 0, 0, 1, 2);
    // add_triangles_vertex(triangles_data, cube_vertex, 1, 0, 3, 2);
    // add_triangles_vertex(triangles_data, cube_vertex, 2, 1, 5, 6);
    // add_triangles_vertex(triangles_data, cube_vertex, 3, 1, 2, 6);
    // add_triangles_vertex(triangles_data, cube_vertex, 4, 5, 4, 7);
    // add_triangles_vertex(triangles_data, cube_vertex, 5, 5, 6, 7);
    // add_triangles_vertex(triangles_data, cube_vertex, 6, 4, 0, 3);
    // add_triangles_vertex(triangles_data, cube_vertex, 7, 4, 7, 3);
    // add_triangles_vertex(triangles_data, cube_vertex, 8, 6, 7, 3);
    // add_triangles_vertex(triangles_data, cube_vertex, 9, 6, 2, 3);
    // add_triangles_vertex(triangles_data, cube_vertex, 10, 1, 0, 4);
    // add_triangles_vertex(triangles_data, cube_vertex, 11, 1, 5, 4);

    // add_triangles_vertex(triangles_color, cube_color, 0, 0, 1, 2);
    // add_triangles_vertex(triangles_color, cube_color, 1, 0, 3, 2);
    // add_triangles_vertex(triangles_color, cube_color, 2, 1, 5, 6);
    // add_triangles_vertex(triangles_color, cube_color, 3, 1, 2, 6);
    // add_triangles_vertex(triangles_color, cube_color, 4, 5, 4, 7);
    // add_triangles_vertex(triangles_color, cube_color, 5, 5, 6, 7);
    // add_triangles_vertex(triangles_color, cube_color, 6, 4, 0, 3);
    // add_triangles_vertex(triangles_color, cube_color, 7, 4, 7, 3);
    // add_triangles_vertex(triangles_color, cube_color, 8, 6, 7, 3);
    // add_triangles_vertex(triangles_color, cube_color, 9, 6, 2, 3);
    // add_triangles_vertex(triangles_color, cube_color, 10, 1, 0, 4);
    // add_triangles_vertex(triangles_color, cube_color, 11, 1, 5, 4);
    
    printf("Buffer Data.\n");
    add_triangles_vertex(triangles_data, cube_vertex, 0, 0, 4, 6);
    add_triangles_vertex(triangles_data, cube_vertex, 1, 0, 6, 2);
    add_triangles_vertex(triangles_data, cube_vertex, 2, 3, 2, 6);
    add_triangles_vertex(triangles_data, cube_vertex, 3, 3, 6, 7);
    add_triangles_vertex(triangles_data, cube_vertex, 4, 7, 6, 4);
    add_triangles_vertex(triangles_data, cube_vertex, 5, 7, 4, 5);
    add_triangles_vertex(triangles_data, cube_vertex, 6, 5, 1, 3);
    add_triangles_vertex(triangles_data, cube_vertex, 7, 5, 3, 7);
    add_triangles_vertex(triangles_data, cube_vertex, 8, 1, 0, 2);
    add_triangles_vertex(triangles_data, cube_vertex, 9, 1, 2, 3);
    add_triangles_vertex(triangles_data, cube_vertex, 10, 5, 4, 0);
    add_triangles_vertex(triangles_data, cube_vertex, 11, 5, 0, 1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj.vertices.size(), &obj.vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    add_UV_vertex(triangles_UV, UV, 0, 0, 4, 8);
    add_UV_vertex(triangles_UV, UV, 1, 0, 8, 2);
    add_UV_vertex(triangles_UV, UV, 2, 3, 2, 9);
    add_UV_vertex(triangles_UV, UV, 3, 3, 9, 11);
    add_UV_vertex(triangles_UV, UV, 4, 12, 10, 5);
    add_UV_vertex(triangles_UV, UV, 5, 12, 5, 7);
    add_UV_vertex(triangles_UV, UV, 6, 6, 1, 3);
    add_UV_vertex(triangles_UV, UV, 7, 6, 3, 13);
    add_UV_vertex(triangles_UV, UV, 8, 1, 0, 2);
    add_UV_vertex(triangles_UV, UV, 9, 1, 2, 3);
    add_UV_vertex(triangles_UV, UV, 10, 7, 5, 0);
    add_UV_vertex(triangles_UV, UV, 11, 7, 0, 1);
    glGenBuffers(1, &VertexColorBufffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexColorBufffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * obj.uvs.size(), &obj.uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, &VertexNormBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexNormBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * obj.normals.size(), &obj.normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void draw() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDrawArrays(GL_TRIANGLES, 0, obj.vertices.size());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

int main() {
    // printf(obj.status?"obj loaded successfully\n":"obj loaded failed\n");
    // Initialize glfw
    if (!glfwInit()) {
        std::cout << "Failed to initialize glfw" << std::endl;
    }
    glfwSetErrorCallback(error_callback);

    // Create a window, and make it current context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1440;
    int height = 900;
    GLFWwindow * window = glfwCreateWindow(width, height, "My Title", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // gladLoadGL (from current context)
    if (!gladLoadGL()) {
        std::cout << "gladLoadGL failed" << std::endl;
    }

    ShaderStage stages[2] = {StageVertex, StageFragment};
    const char * filePaths[2] = {"shader/vertex.glsl", "shader/fragment.glsl"};

    ModelObject obj1 = ModelObject("cube.obj");
    glm::mat4 trans_1 = glm::mat4(
        glm::vec4(0.75, 0., 0., 0.),
        glm::vec4(0., 0.75, 0., 0.),
        glm::vec4(0., 0., 0.75, 0.),
        glm::vec4(0., 0., 0., 1.0)
    );
    auto proj_1 = [trans_1](glm::vec3 x) -> glm::vec3 {
        glm::vec4 x4 = glm::vec4(x.x, x.y, x.z, 1.0);
        x4 = trans_1 * x4;
        glm::vec3 x3 = glm::vec3(x4.x, x4.y, x4.z);
        return x3;
    };
    obj1.apply(proj_1);

    ModelObject obj2 = ModelObject("cube.obj");
    glm::mat4 trans_2 = glm::mat4(1.0);
    // for (int i = 0; i < 4; ++i) {
    //     for (int j = 0; j < 4; ++j) {
    //         std::cout << trans_2[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
    trans_2 = glm::translate(trans_2, glm::vec3(0.875, 0., 0.));
    // for (int i = 0; i < 4; ++i) {
    //     for (int j = 0; j < 4; ++j) {
    //         std::cout << trans_2[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
    trans_2 = glm::scale(trans_2, glm::vec3(0.125, 0.125, 0.125));
    // for (int i = 0; i < 4; ++i) {
    //     for (int j = 0; j < 4; ++j) {
    //         std::cout << trans_2[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
    auto proj_2 = [trans_2](glm::vec3 x) -> glm::vec3 {
        glm::vec4 x4 = glm::vec4(x.x, x.y, x.z, 1.0);
        // std::cout << x4[0] << ' ' << x4[1] << ' ' << x4[2] << ' ' << x4[3] << " fuck-1" << std::endl;
        x4 = trans_2 * x4;
        // std::cout << x4[0] << ' ' << x4[1] << ' ' << x4[2] << ' ' << x4[3] << " fuck-2" << std::endl;
        glm::vec3 x3 = glm::vec3(x4.x, x4.y, x4.z);
        return x3;
    };
    obj2.apply(proj_2);
    for (int i = 0; i < obj2.vertices.size(); ++i) {
        std::cout << obj2.vertices[i][0] << ' ' << obj2.vertices[i][1] << ' ' <<obj2.vertices[i][2] << std::endl;
    }

    obj.append(obj1);
    obj.append(obj2);
    std::cout << obj.vertices.size() << std::endl;

    init();
    printf("Initialized.\n");
    GLuint programID = LoadShaders(2, stages, filePaths);

    glUseProgram(programID);

    GlobalProjection = new glm::mat4(glm::perspective(
        glm::radians(45.f),
        ((GLfloat)width) / ((GLfloat)height),
        0.1f, 
        100.f
    ));
    GlobalEye = new glm::vec3(4., 3., 3.);
    GlobalView = new glm::mat4(glm::lookAt(
        *GlobalEye,
        glm::vec3(0., 0., 0.),
        glm::vec3(0., 1., 0.)
    ));
    GlobalModel = new glm::mat4(1.f);
    GlobalMVP = new glm::mat4((*GlobalProjection) * (*GlobalView) * (*GlobalModel));

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint EyeID = glGetUniformLocation(programID, "Eye");
    GLuint LightID = glGetUniformLocation(programID, "Light");
    // glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (GLfloat*)GlobalMVP);

    glm::vec3 light_pos = glm::vec3(3., 3., 3.);

    GLuint textureID = LoadBMPTexture("./city-sun.bmp");
    printf("ImageTexture Loaded.\n");
    // Draw
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (GLfloat*)GlobalMVP);
        glUniform3fv(EyeID, 1, (GLfloat*)GlobalEye);
        glUniform3fv(LightID, 1, (GLfloat*)&light_pos[0]);
        draw();
        // std::cout << GlobalEye->x << ' ' << GlobalEye->y << ' ' << GlobalEye->z << std::endl;
        glfwSwapBuffers(window);
        glfwPollEvents();
        *GlobalEye = RotateStepXOZ * (*GlobalEye);
        // updateMVP();
    }

    // Exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
