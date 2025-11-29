#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <cstdint>
#include <vector>
#include <iostream>

inline unsigned char* stbi_load(const char* filename,int* x,int* y,int* comp,int req_comp){
    std::cerr<<"[stb_image] placeholder: image loading not implemented.\n";
    *x = *y = *comp = 1;
    static unsigned char dummy[4] = {255,255,255,255};
    return dummy;
}
inline void stbi_image_free(void* ptr){}
