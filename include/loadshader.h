#ifndef LOADSHADER_H
#define LOADSHADER_H

#include <glad/glad.h>

enum ShaderStage {StageVertex, StageFragment, StageNums};

GLuint LoadShaders(int n, ShaderStage shaderStages[], const char * shaderPaths[]);

#endif