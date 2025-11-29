#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "stb_image.h"
#include "tiny_gltf_wrapper.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <GLES2/gl2.h>

static GLuint upload_texture(const unsigned char* pixels, int w, int h, int comp){
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum format = (comp==4)?GL_RGBA:GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    return tex;
}

bool load_gltf_model(const std::string &path, std::vector<MeshGL> &out_meshes){
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;
    bool ret = false;
    if(path.size()>4 && path.substr(path.size()-4)==".glb"){
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    } else {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }
    if(!warn.empty()) std::cerr<<"gltf warn: "<<warn<<"\n";
    if(!err.empty()) std::cerr<<"gltf err: "<<err<<"\n";
    if(!ret){ std::cerr<<"Failed to load gltf\n"; return false; }

    for(const auto &mesh : model.meshes){
        for(const auto &prim : mesh.primitives){
            if(prim.attributes.find("POSITION")==prim.attributes.end()) continue;
            const tinygltf::Accessor& posAcc = model.accessors[prim.attributes.find("POSITION")->second];
            const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
            const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
            const void* posData = &(posBuffer.data[posView.byteOffset + posAcc.byteOffset]);

            bool hasTex = prim.attributes.count("TEXCOORD_0")>0;
            const void* texData = nullptr;
            if(hasTex){
                const tinygltf::Accessor& tcAcc = model.accessors[prim.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& tcView = model.bufferViews[tcAcc.bufferView];
                const tinygltf::Buffer& tcBuffer = model.buffers[tcView.buffer];
                texData = &(tcBuffer.data[tcView.byteOffset + tcAcc.byteOffset]);
            }

            const tinygltf::Accessor& idxAcc = model.accessors[prim.indices];
            const tinygltf::BufferView& idxView = model.bufferViews[idxAcc.bufferView];
            const tinygltf::Buffer& idxBuffer = model.buffers[idxView.buffer];
            const void* idxData = &(idxBuffer.data[idxView.byteOffset + idxAcc.byteOffset]);

            int vertexCount = posAcc.count;
            std::vector<float> interleaved;
            interleaved.reserve(vertexCount * 5);
            for(int i=0;i<vertexCount;i++){
                const float* p = ((const float*)posData) + 3*i;
                interleaved.push_back(p[0]); interleaved.push_back(p[1]); interleaved.push_back(p[2]);
                if(hasTex){
                    const float* t = ((const float*)texData) + 2*i;
                    interleaved.push_back(t[0]); interleaved.push_back(t[1]);
                } else {
                    interleaved.push_back(0.0f); interleaved.push_back(0.0f);
                }
            }

            MeshGL mg;
            glGenBuffers(1, &mg.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mg.vbo);
            glBufferData(GL_ARRAY_BUFFER, interleaved.size()*sizeof(float), interleaved.data(), GL_STATIC_DRAW);

            mg.index_count = idxAcc.count;
            glGenBuffers(1, &mg.ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mg.ibo);
            if(idxAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT){
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxAcc.count * sizeof(unsigned short), idxData, GL_STATIC_DRAW);
            } else {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxAcc.count * sizeof(unsigned int), idxData, GL_STATIC_DRAW);
            }

            if(prim.material >= 0){
                const tinygltf::Material& mat = model.materials[prim.material];
                auto it = mat.values.find("baseColorTexture");
                if(it != mat.values.end()){
                    int texIndex = it->second.TextureIndex();
                    if(texIndex >= 0){
                        const tinygltf::Image &img = model.images[ model.textures[texIndex].source ];
                        int w = img.width, h = img.height, comp = img.component;
                        if(img.image.size()>0){
                            mg.tex = upload_texture(img.image.data(), w, h, comp==4?4:3);
                            mg.has_texcoord = true;
                        }
                    }
                }
            }

            out_meshes.push_back(mg);
        }
    }
    return true;
}
