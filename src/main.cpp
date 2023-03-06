#include "glm/trigonometric.hpp"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <loadshader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define V_POSITION 0

void error_callback(int error, const char * description){
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_data), triangles_data, GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_UV), triangles_UV, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
}

void draw() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

int main() {
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
    GLFWwindow * window = glfwCreateWindow(2560, 1600, "My Title", NULL, NULL);
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
    
    init();
    printf("Initialized.\n");
    GLuint programID = LoadShaders(2, stages, filePaths);

    glUseProgram(programID);

    glm::mat4 Projection = glm::perspective(
        glm::radians(45.f),
        2560.f / 1600.f,
        0.1f, 
        100.f
    );
    glm::mat4 View = glm::lookAt(
        glm::vec3(4., 3., 3.),
        glm::vec3(0., 0., 0.),
        glm::vec3(0., 1., 0.)
    );
    glm::mat4 Model = glm::mat4(1.f);
    glm::mat4 mvp = Projection * View * Model;

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    GLuint textureID = LoadBMPTexture("./lake.bmp");
    printf("ImageTexture Loaded.\n");
    // Draw
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
