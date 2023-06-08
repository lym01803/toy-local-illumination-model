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

enum TextureType {shadowmap, shadowmap2, nummaps};

GLuint TextureIDs[nummaps] = {0xffffffff};

GLuint VertexBuffer;
GLuint VertexArrayID;
GLuint VertexColorBuffer;
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

int initVBOs(ModelObject * objs[], int n, int * size_count, 
                GLuint * vertexbuffer, GLuint * vertexarrayid, GLuint * vertexuvbuffer, GLuint * vertexnormbuffer) {
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

    glGenBuffers(1, vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vertexbuffer);

    glGenVertexArrays(1, vertexarrayid);
    glBindVertexArray(*vertexarrayid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (*size_count), merged_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, vertexuvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vertexuvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * (*size_count), merged_uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, vertexnormbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, *vertexnormbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * (*size_count), merged_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    free(merged_vertices);
    free(merged_uvs);
    free(merged_normals);
    return 1;
}

void draw(int size_count, int enable_depth_test = 1, int clear = 1, int blend = 0) {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnable(GL_TEXTURE_2D);
    if (clear) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    if (enable_depth_test) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_DEPTH_TEST);
        // glDisable(GL_CULL_FACE);
    }
    if (blend) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        glDisable(GL_BLEND);
    }
    glDrawArrays(GL_TRIANGLES, 0, size_count);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void bind_shadow_map(int index, GLuint * depth_fbo_ID_ptr) {
    GLuint& tid = TextureIDs[index];
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
    // glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Fuck you!!!!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main() {
    // ========================== Initialize glfw =================================
    if (!glfwInit()) {
        std::cout << "Failed to initialize glfw" << std::endl;
    }
    glfwSetErrorCallback(error_callback);

    // Create a window, and make it current context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1920;
    int height = 1200;
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
    // =========================== Initialize VBO ==================================
    int vertex_count;

    std::vector<ModelObject> objlist = vector<ModelObject>();
    loadObjectsfromTxt("scene.txt", objlist);
    std::cout << "Loaded: " << objlist.size() << " objects." << std::endl;

    ModelObject ** objs_ptr_arr = (ModelObject **) malloc(sizeof(ModelObject *) * objlist.size());
    for (int i = 0; i < objlist.size(); ++i) {
        objs_ptr_arr[i] = &objlist[i];
    }

    initVBOs(objs_ptr_arr, objlist.size(), &vertex_count,
            &VertexBuffer, &VertexArrayID, &VertexColorBuffer, &VertexNormBuffer);

    std::vector<ModelObject> objlist2 = vector<ModelObject>();
    loadObjectsfromTxt("scene-2.txt", objlist2);
    ModelObject ** objs_ptr_arr2 = (ModelObject **) malloc(sizeof(ModelObject *) * objlist2.size());
    for (int i = 0; i < objlist2.size(); ++i) {
        objs_ptr_arr2[i] = &objlist2[i];
    }

    int vertex_count2;
    GLuint VertexBuffer2;
    GLuint VertexArrayID2;
    GLuint VertexColorBuffer2;
    GLuint VertexNormBuffer2;
    initVBOs(objs_ptr_arr2, objlist2.size(), &vertex_count2, 
            &VertexBuffer2, &VertexArrayID2, &VertexColorBuffer2, &VertexNormBuffer2);
    
    // GLuint textureID = LoadBMPTexture("./city-sun.bmp");
    // =============================== Light ===================================

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

    glm::vec3 light_pos2 = glm::vec3(-4., 4., 3.);
    glm::mat4 light_view_matrix2 = glm::mat4(
        glm::lookAt(light_pos2, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0))
    );
    glm::mat4 light_project_matrix2 = glm::mat4(glm::perspective(
        glm::radians(85.f),
        1.0f,
        0.1f, 
        100.f
    ));
    glm::mat4 shadow_mvp2 = light_project_matrix2 * light_view_matrix2;

    // ============================ Shadow Map Texture Buffer ==============================
    GLuint depth_fbo_ID[nummaps];
    memset(depth_fbo_ID, 0xff, sizeof(GLuint) * nummaps);
    bind_shadow_map(shadowmap, &depth_fbo_ID[shadowmap]);
    bind_shadow_map(shadowmap2, &depth_fbo_ID[shadowmap2]);
    std::cout << TextureIDs[shadowmap] << ' ' << depth_fbo_ID[shadowmap] << ' ' << std::endl;
    std::cout << TextureIDs[shadowmap2] << ' ' << depth_fbo_ID[shadowmap2] << ' ' << std::endl;

    // ============================== Draw Shadow Map ===============================
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

    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo_ID[shadowmap]);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VertexArrayID);
    draw(vertex_count);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUniformMatrix4fv(
        glGetUniformLocation(shadow_programID, "MVP"),
        1,
        GL_FALSE,
        (GLfloat*)&shadow_mvp2
    );
    // ============================= Repeat for Light Source 2 ===============================
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo_ID[shadowmap2]);
    glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VertexArrayID);
    draw(vertex_count);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ============================= Prepare for Shader ===============================
    ShaderStage stages[2] = {StageVertex, StageFragment};
    const char * filePaths[2] = {"shader/vertex.glsl", "shader/fragment.glsl"};

    GLuint programID = LoadShaders(2, stages, filePaths);

    glUseProgram(programID);

    // ============================= Model View Projection ==============================
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

    // ============================= Prepare for Uniform Variable Passing ================================
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint EyeID = glGetUniformLocation(programID, "Eye");
    GLuint LightID = glGetUniformLocation(programID, "Light");
    GLuint LightID2 = glGetUniformLocation(programID, "Light2");
    GLuint ShadowMatrix1ID = glGetUniformLocation(programID, "shadow_matrix1");
    GLuint ShadowMatrix1ID2 = glGetUniformLocation(programID, "shadow_matrix2");
    GLuint FlagID = glGetUniformLocation(programID, "flag");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureIDs[shadowmap]);
    glUniform1i(glGetUniformLocation(programID, "depth_texture"), shadowmap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TextureIDs[shadowmap2]);
    glUniform1i(glGetUniformLocation(programID, "depth_texture2"), shadowmap2);

    glm::mat4 shadow_matrix1 = glm::mat4(
        glm::vec4(0.5, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.5, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 0.5, 0.0),
        glm::vec4(0.5, 0.5, 0.5, 1.0)
    ) * shadow_mvp;
    glm::mat4 shadow_matrix2 = glm::mat4(
        glm::vec4(0.5, 0.0, 0.0, 0.0),
        glm::vec4(0.0, 0.5, 0.0, 0.0),
        glm::vec4(0.0, 0.0, 0.5, 0.0),
        glm::vec4(0.5, 0.5, 0.5, 1.0)
    ) * shadow_mvp2;

    printf("ImageTexture Loaded.\n");
    // Draw
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    double currentTime = glfwGetTime();
    glm::vec4 posflag = glm::vec4(1.0);
    glm::vec4 negflag = glm::vec4(-1.0);

    // ========================= Main Loop =============================
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        // printf("%d %d\n", width, height);
        // ======================= Update M V P According to Current Eye Position ========================
        glViewport(0, 0, width, height);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, (GLfloat*)GlobalMVP);
        glUniform3fv(EyeID, 1, (GLfloat*)GlobalEye);
        glUniform3fv(LightID, 1, (GLfloat*)&light_pos[0]);
        glUniform3fv(LightID2, 1, (GLfloat*)&light_pos2[0]);
        glUniformMatrix4fv(ShadowMatrix1ID, 1, GL_FALSE, (GLfloat*)&shadow_matrix1);
        glUniformMatrix4fv(ShadowMatrix1ID2, 1, GL_FALSE, (GLfloat*)&shadow_matrix2);
        // ============================== Bind and Draw Call ===============================
        glUniform4fv(FlagID, 1, (GLfloat*)&posflag[0]);
        glBindVertexArray(VertexArrayID);
        glDepthMask(GL_TRUE);
        draw(vertex_count, 1, 1, 0);
        
        glUniform4fv(FlagID, 1, (GLfloat*)&negflag[0]);
        glBindVertexArray(VertexArrayID2);
        glDepthMask(GL_FALSE);
        draw(vertex_count2, 1, 0, 1);
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
