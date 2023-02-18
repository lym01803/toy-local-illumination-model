#include <cstdio>
#include <cstdlib>
#include <loadshader.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#define LOAD_SHADER(STAGE) \
    GLuint STAGE##ShaderID = glCreateShader(GL_##STAGE##_SHADER);\
    std::string STAGE##ShaderCode;\
    std::ifstream STAGE##ShaderStream(filepath, std::ios::in);\
    if (STAGE##ShaderStream.is_open()) {\
        std::stringstream sstr;\
        sstr << STAGE##ShaderStream.rdbuf();\
        STAGE##ShaderCode = sstr.str();\
        STAGE##ShaderStream.close();\
    }\
    printf("Compiling shader : %s\n", filepath);\
    char const * STAGE##SourcePtr = STAGE##ShaderCode.c_str();\
    glShaderSource(STAGE##ShaderID, 1, & STAGE##SourcePtr, NULL);\
    glCompileShader(STAGE##ShaderID);\
    \
    GLint STAGE##Result = GL_FALSE;\
    int STAGE##InfoLogLength;\
    glGetShaderiv(STAGE##ShaderID, GL_COMPILE_STATUS, & STAGE##Result);\
    glGetShaderiv(STAGE##ShaderID, GL_INFO_LOG_LENGTH, & STAGE##InfoLogLength);\
    if (STAGE##InfoLogLength > 0) {\
        char STAGE##ShaderErr[STAGE##InfoLogLength + 1];\
        glGetShaderInfoLog(STAGE##ShaderID, STAGE##InfoLogLength, NULL, STAGE##ShaderErr);\
        printf("%s\n", STAGE##ShaderErr);\
    }


GLuint LoadShaders(int n, ShaderStage *shaderStages, const char **shaderPaths) {
    const char * filepath;
    GLuint ProgramID = glCreateProgram();
    GLuint ShaderIDArr[StageNums];
    for (int i = 0; i < n; ++i) {
        filepath = shaderPaths[i];
        switch (shaderStages[i]) {
            case StageVertex: {
                LOAD_SHADER(VERTEX)
                ShaderIDArr[StageVertex] = VERTEXShaderID;
                glAttachShader(ProgramID, ShaderIDArr[StageVertex]);
                break;
            }
            case StageFragment: {
                LOAD_SHADER(FRAGMENT)
                ShaderIDArr[StageFragment] = FRAGMENTShaderID;
                glAttachShader(ProgramID, ShaderIDArr[StageFragment]);
                break;
            }
            default : { }
        }
    }
    GLint Result;
    int InfoLogLength;

    glLinkProgram(ProgramID);
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        char ErrMsg[InfoLogLength + 1];
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ErrMsg);
        printf("%s\n", ErrMsg);
    }

    for (int i = 0; i < n; ++i) {
        glDetachShader(ProgramID, ShaderIDArr[shaderStages[i]]);
        glDeleteShader(ShaderIDArr[shaderStages[i]]);
    }

    return ProgramID;
}
