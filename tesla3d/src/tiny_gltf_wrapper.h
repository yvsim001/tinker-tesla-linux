#pragma once
#include <string>
#include <vector>
#include <GLES2/gl2.h>

struct MeshGL {
    GLuint vbo = 0;
    GLuint ibo = 0;
    int index_count = 0;
    bool has_texcoord = false;
    GLuint tex = 0;
};

bool load_gltf_model(const std::string &path, std::vector<MeshGL> &out_meshes);
