#pragma once
#include <GLES2/gl2.h>
#include <string>
GLuint compile_shader(GLenum type, const char* src);
GLuint link_program(GLuint vs, GLuint fs);
std::string load_text_file(const char* path);
