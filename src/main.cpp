#include "line.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <loadshader.h>

#define V_POSITION 0

GLuint VertexBuffer, VertexArrayID;
GLfloat a = 1., b = 0., c = 0., xb = -1., xe = 1.;

void error_callback(int error, const char * description){
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    GLfloat h = 0.01;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        c += h * (b / a) * (b / a) * 0.25;
        b += h * b / a;
        a += h;
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        if (a > 5 * h) {
            c -= h * (b / a) * (b / a) * 0.25;
            b -= h * b / a;
            a -= h;
        }
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        c += h * b + h * h * a;
        b += h * 2 * a;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        c -= h * b + h * h * a;
        b -= h * 2 * a;
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        c += h;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        c -= h;
    }
}

void init() {
    // GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    printf("Vertex Array Bound.\n");

    // GLuint VertexBuffer;
    glGenBuffers(1, &VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    printf("Buffer Bound.\n");

    static const GLfloat vertex_data[] = {
        -1.f, -1.f,
        1.f, -1.f, 
        0.f, 0.f,
        1.f, 1.f,
        0.f, 0.f,
        -1.f, 1.f,
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    printf("Buffer Data.\n");

    glVertexAttribPointer(V_POSITION, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
}

void draw() {
    glEnableVertexAttribArray(V_POSITION);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0., 0., 0., 0.);
    glBindVertexArray(VertexArrayID);
    glDrawArrays(GL_LINES, 0, 6);
    glDisableVertexAttribArray(V_POSITION);
}

void draw2(GLfloat * arr, int length) {
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glGenVertexArrays(1, &VertexArrayID);
    // glBindVertexArray(VertexArrayID);
    
    // glGenBuffers(1, &VertexBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * length, arr, GL_STATIC_DRAW);
    
    glVertexAttribPointer(V_POSITION, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    
    glEnableVertexAttribArray(V_POSITION);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(.2, 0., 0.5, 1.);
    glBindVertexArray(VertexArrayID);
    glDrawArrays(GL_LINES, 0, length >> 1);
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
    GLFWwindow * window = glfwCreateWindow(640, 640, "My Title", NULL, NULL);
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
    int border = 20;
    // GLfloat a = 1., b = 0., c = 0., xb = -1., xe = 1.;
    // std::cin >> a >> b >> c >> xb >> xe;

    GLfloat step = 2.f / 600.f;
    int numVertex = (xe - xb) / step * 4 + 4;
    GLfloat * vertex_buffer = (GLfloat *)malloc(sizeof(GLfloat) * numVertex);
    numVertex = getParabolaVertexArray(vertex_buffer, a, b, c, xb, xe, step);
    // std::cout << numVertex << std::endl;

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(border, border, width - 2 * border, height - 2 * border);
        numVertex = getParabolaVertexArray(vertex_buffer, a, b, c, xb, xe, step);
        draw2(vertex_buffer, numVertex);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
