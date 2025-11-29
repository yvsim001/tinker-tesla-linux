#include "gl_helpers.h"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint compile_shader(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char buf[1024]; glGetShaderInfoLog(s,1024,NULL,buf);
        std::cerr << "Shader compile error: " << buf << std::endl;
    }
    return s;
}

GLuint link_program(GLuint vs, GLuint fs){
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){
        char buf[1024]; glGetProgramInfoLog(p,1024,NULL,buf);
        std::cerr << "Program link error: " << buf << std::endl;
    }
    return p;
}

std::string load_text_file(const char* path){
    std::ifstream ifs(path);
    std::stringstream ss; ss << ifs.rdbuf();
    return ss.str();
}
