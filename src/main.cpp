#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <loadshader.h>
#include <glm/glm.hpp>

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

void init() {
    GLuint VertexBuffer;
    glGenBuffers(1, &VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    printf("Buffer Bound.\n");

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    printf("Vertex Array Bound.\n");

    static const GLfloat vertex_data[] = {
        -1.f, -1.f,
        1.f, -1.f, 
        0.f, 1.f,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    printf("Buffer Data.\n");

    glVertexAttribPointer(V_POSITION, 2, GL_FLOAT, GL_FALSE, 0, (void *)NULL);
}

void draw() {
    glEnableVertexAttribArray(V_POSITION);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(V_POSITION);
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
