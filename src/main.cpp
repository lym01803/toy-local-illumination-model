#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include <cstddef>
#include <glad/glad.h>
#include <ostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <loadshader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <loadobj.h>
#include <vector>

#define V_POSITION 0
#define DEPTH_TEXTURE_SIZE 4096

enum TextureType {shadowmap, nummaps};

GLuint TextureIDs[nummaps] = {0xffffffff};

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

GLfloat theta = acos(-1.) / 60;
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

int initVBOs(ModelObject * objs[], int n, int * size_count) {
    *size_count = 0;
    for (int i = 0; i < n; ++i) {
        *size_count += objs[i]->vertices.size();
    }
    GLfloat * merged_vertices = (GLfloat *) malloc(sizeof(glm::vec3) * (*size_count));
    GLfloat * merged_uvs = (GLfloat *) malloc(sizeof(glm::vec2) * (*size_count));
    GLfloat * merged_normals = (GLfloat *) malloc(sizeof(glm::vec3) * (*size_count));

    for (int i = 0, offset = 0; i < n; ++i) {
        memcpy(merged_vertices + offset * 3, &(objs[i]->vertices[0]), sizeof(glm::vec3) * objs[i]->vertices.size());
        memcpy(merged_uvs + offset * 2, &(objs[i]->uvs[0]), sizeof(glm::vec2) * objs[i]->uvs.size());
        memcpy(merged_normals + offset * 3, &(objs[i]->normals[0]), sizeof(glm::vec3) * objs[i]->normals.size());
        offset += objs[i]->vertices.size();
    }

    glGenBuffers(1, &VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (*size_count), merged_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, &VertexColorBufffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexColorBufffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * (*size_count), merged_uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, &VertexNormBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexNormBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (*size_count), merged_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    free(merged_vertices);
    free(merged_uvs);
    free(merged_normals);
    return 1;
}

void draw(int size_count) {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDrawArrays(GL_TRIANGLES, 0, size_count);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void shadow_map(GLuint * depth_fbo_ID_ptr) {
    GLuint& tid = TextureIDs[shadowmap];
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, depth_fbo_ID_ptr);
    glBindFramebuffer(GL_FRAMEBUFFER, *depth_fbo_ID_ptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depth_fbo_ID_ptr, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Fuck you!!!!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    // =============================================================================
    int vertex_count;

    std::vector<ModelObject> objlist = vector<ModelObject>();
    loadObjectsfromTxt("scene.txt", objlist);
    std::cout << "Loaded: " << objlist.size() << " objects." << std::endl;

    ModelObject ** objs_ptr_arr = (ModelObject **) malloc(sizeof(ModelObject *) * objlist.size());
    for (int i = 0; i < objlist.size(); ++i) {
        objs_ptr_arr[i] = &objlist[i];
    }

    initVBOs(objs_ptr_arr, objlist.size(), &vertex_count);

    // GLuint textureID = LoadBMPTexture("./city-sun.bmp");

    glm::vec3 light_pos = glm::vec3(2., 4., 3.);
    glm::mat4 light_view_matrix = glm::mat4(
        glm::lookAt(light_pos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0))
    );
    glm::mat4 light_project_matrix = glm::mat4(glm::perspective(
        glm::radians(85.f),
        1.0f,
        0.1f, 
        100.f
    ));
    glm::mat4 shadow_mvp = light_project_matrix * light_view_matrix;

    ShaderStage shadow_stages[2] = {StageVertex, StageFragment};
    const char * shadow_filePaths[2] = {"shader/shadowmap_vertex.glsl", "shader/shadowmap_fragment.glsl"};

    GLuint shadow_programID = LoadShaders(2, shadow_stages, shadow_filePaths);

    glUseProgram(shadow_programID);
    glUniformMatrix4fv(
        glGetUniformLocation(shadow_programID, "MVP"),
        1,
        GL_FALSE,
        (GLfloat*)&shadow_mvp
    );
    
    GLuint depth_fbo_ID = 0xffffffff;
    shadow_map(&depth_fbo_ID);
    std::cout << TextureIDs[shadowmap] << ' ' << depth_fbo_ID << ' ' << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo_ID);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);

    draw(vertex_count);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ShaderStage stages[2] = {StageVertex, StageFragment};
    const char * filePaths[2] = {"shader/vertex.glsl", "shader/fragment.glsl"};

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
    GLuint ShadowMatrix1ID = glGetUniformLocation(programID, "shadow_matrix1");

    glm::mat4 shadow_matrix1 = glm::mat4(
        glm::vec4(0.5, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.5, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 0.5, 0.0),
        glm::vec4(0.5, 0.5, 0.5, 1.0)
    ) * shadow_mvp;

    printf("ImageTexture Loaded.\n");
    // Draw
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    double currentTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        // printf("%d %d\n", width, height);
        glViewport(0, 0, width, height);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (GLfloat*)GlobalMVP);
        glUniform3fv(EyeID, 1, (GLfloat*)GlobalEye);
        glUniform3fv(LightID, 1, (GLfloat*)&light_pos[0]);
        glUniformMatrix4fv(ShadowMatrix1ID, 1, GL_FALSE, (GLfloat*)&shadow_matrix1);
        draw(vertex_count);
        // std::cout << GlobalEye->x << ' ' << GlobalEye->y << ' ' << GlobalEye->z << std::endl;
        glfwSwapBuffers(window);
        glfwPollEvents();
        // *GlobalEye = RotateStepXOZ * (*GlobalEye);
        // updateMVP();
        ++nbFrames;
        currentTime = glfwGetTime();
        if (currentTime - lastTime >= 1.0) {
            printf("%lf ms/frame; %lf frames/sec\n", 1000.0 * (currentTime - lastTime)/double(nbFrames),\
                    double(nbFrames) / (currentTime - lastTime));
            nbFrames = 0;
            lastTime = glfwGetTime();
        }
    }

    // Exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
