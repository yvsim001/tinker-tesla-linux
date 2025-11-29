#pragma once
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>
#include <cstring>

namespace tinygltf {

struct Image { std::vector<unsigned char> image; int width=0,height=0,component=4; };
struct Texture { int source=-1; };
struct Material { std::map<std::string,int> values; };
struct Accessor { int bufferView=-1; int count=0; int componentType=0; };
struct BufferView { int buffer=-1; int byteOffset=0; };
struct Buffer { std::vector<unsigned char> data; };
struct Primitive { std::map<std::string,int> attributes; int indices=-1; int material=-1; };
struct Mesh { std::vector<Primitive> primitives; };
struct Model { std::vector<Mesh> meshes; std::vector<Image> images; std::vector<Texture> textures; std::vector<Material> materials; std::vector<Accessor> accessors; std::vector<BufferView> bufferViews; std::vector<Buffer> buffers; };
class TinyGLTF {
public:
    bool LoadBinaryFromFile(Model* model, std::string* err, std::string* warn, const std::string& filename){
        std::cerr<<"[tiny_gltf] GLB load placeholder: use real tiny_gltf.h for full GLB support.\n"; return false;
    }
    bool LoadASCIIFromFile(Model* model, std::string* err, std::string* warn, const std::string& filename){ return false; }
};

#define TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT 5123
} // namespace tinygltf
