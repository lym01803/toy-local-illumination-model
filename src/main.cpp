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

static GLfloat cube_vertex[8 * 3] = {
    -1.f, -1.f, -1.f,
    1.f, -1.f, -1.f,
    1.f, 1.f, -1.f,
    -1.f, 1.f, -1.f,
    -1.f, -1.f, 1.f, 
    1.f, -1.f, 1.f,
    1.f, 1.f, 1.f,
    -1.f, 1.f, 1.f
};

static GLfloat cube_color[8 * 3] = {
    1., 1., .3,
    1., .3, 1.,
    .3, 1., 1.,
    1., .3, .3,
    .3, 1., .3,
    .3, .3, 1.,
    .7, 1., .3,
    .3, 1., .7
};

void add_triangles_vertex(GLfloat * triangles, GLfloat * vertex, int idTri, int idA, int idB, int idC) {
    GLfloat * start = triangles + idTri * 9;
    memcpy(start + 0, vertex + idA * 3, sizeof(GLfloat) * 3);
    memcpy(start + 3, vertex + idB * 3, sizeof(GLfloat) * 3);
    memcpy(start + 6, vertex + idC * 3, sizeof(GLfloat) * 3);
}

GLfloat triangles_data[3 * 3 * 12];
GLfloat triangles_color[3 * 3 * 12];

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
    add_triangles_vertex(triangles_data, cube_vertex, 0, 0, 1, 2);
    add_triangles_vertex(triangles_data, cube_vertex, 1, 0, 2, 3);
    add_triangles_vertex(triangles_data, cube_vertex, 2, 0, 3, 4);
    add_triangles_vertex(triangles_data, cube_vertex, 3, 3, 4, 7);
    add_triangles_vertex(triangles_data, cube_vertex, 4, 4, 5, 7);
    add_triangles_vertex(triangles_data, cube_vertex, 5, 5, 6, 7);
    add_triangles_vertex(triangles_data, cube_vertex, 6, 5, 6, 1);
    add_triangles_vertex(triangles_data, cube_vertex, 7, 2, 6, 1);
    add_triangles_vertex(triangles_data, cube_vertex, 8, 3, 6, 7);
    add_triangles_vertex(triangles_data, cube_vertex, 9, 3, 6, 2);
    add_triangles_vertex(triangles_data, cube_vertex, 10, 5, 0, 1);
    add_triangles_vertex(triangles_data, cube_vertex, 11, 5, 0, 4);

    add_triangles_vertex(triangles_color, cube_color, 0, 0, 1, 2);
    add_triangles_vertex(triangles_color, cube_color, 1, 0, 2, 3);
    add_triangles_vertex(triangles_color, cube_color, 2, 0, 3, 4);
    add_triangles_vertex(triangles_color, cube_color, 3, 3, 4, 7);
    add_triangles_vertex(triangles_color, cube_color, 4, 4, 5, 7);
    add_triangles_vertex(triangles_color, cube_color, 5, 5, 6, 7);
    add_triangles_vertex(triangles_color, cube_color, 6, 5, 6, 1);
    add_triangles_vertex(triangles_color, cube_color, 7, 2, 6, 1);
    add_triangles_vertex(triangles_color, cube_color, 8, 3, 6, 7);
    add_triangles_vertex(triangles_color, cube_color, 9, 3, 6, 2);
    add_triangles_vertex(triangles_color, cube_color, 10, 5, 0, 1);
    add_triangles_vertex(triangles_color, cube_color, 11, 5, 0, 4);
    printf("Buffer Data.\n");

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_data), triangles_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, &VertexColorBufffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexColorBufffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_color), triangles_color, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 1, (void *)0);


}

void draw() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
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
    GLFWwindow * window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
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
        640.f / 480.f,
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
