#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "gl_helpers.h"
#include "tiny_gltf_wrapper.h"

static int drm_fd = -1;
static drmModeModeInfo mode;
static struct gbm_device *gbm = nullptr;
static struct gbm_surface *gbm_surf = nullptr;
static EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static EGLContext eglContext = EGL_NO_CONTEXT;
static EGLSurface eglSurface = EGL_NO_SURFACE;

bool init_drm_gbm() {
    drm_fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if(drm_fd < 0){ perror("open drm"); return false; }
    drmModeRes *res = drmModeGetResources(drm_fd);
    for(int i=0;i<res->count_connectors;i++){
        drmModeConnector *conn = drmModeGetConnector(drm_fd, res->connectors[i]);
        if(conn->connection == DRM_MODE_CONNECTED && conn->count_modes>0){
            mode = conn->modes[0];
            drmModeFreeConnector(conn);
            break;
        }
        drmModeFreeConnector(conn);
    }
    gbm = gbm_create_device(drm_fd);
    gbm_surf = gbm_surface_create(gbm, mode.hdisplay, mode.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
    return gbm && gbm_surf;
}

bool init_egl() {
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)gbm);
    if(eglDisplay == EGL_NO_DISPLAY){ std::cerr<<"eglGetDisplay failed\n"; return false; }
    if(!eglInitialize(eglDisplay, NULL, NULL)){ std::cerr<<"eglInitialize failed\n"; return false; }

    EGLint attribs[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };
    EGLConfig cfg;
    EGLint numCfg;
    eglChooseConfig(eglDisplay, attribs, &cfg, 1, &numCfg);
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint ctxAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eglContext = eglCreateContext(eglDisplay, cfg, EGL_NO_CONTEXT, ctxAttribs);
    eglSurface = eglCreateWindowSurface(eglDisplay, cfg, (EGLNativeWindowType)gbm_surf, NULL);
    if(!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)){ std::cerr<<"eglMakeCurrent failed\n"; return false; }
    return true;
}

int main(int argc, char** argv){
    std::string model_path = "/opt/tesla/tesla.glb";
    if(!init_drm_gbm()) return -1;
    if(!init_egl()) return -1;

    std::string vs = load_text_file("shaders/vs.glsl");
    std::string fs = load_text_file("shaders/fs.glsl");
    GLuint vs_s = compile_shader(GL_VERTEX_SHADER, vs.c_str());
    GLuint fs_s = compile_shader(GL_FRAGMENT_SHADER, fs.c_str());
    GLuint program = link_program(vs_s, fs_s);

    std::vector<MeshGL> meshes;
    if(!load_gltf_model(model_path, meshes)){
        std::cerr << "Model load failed\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f,0.05f,0.07f,1.0f);

    GLint locPos = glGetAttribLocation(program, "aPos");
    GLint locTex = glGetAttribLocation(program, "aTex");
    GLint locMVP = glGetUniformLocation(program, "uMVP");
    GLint locLight = glGetUniformLocation(program, "uLightDir");

    float angle = 0.0f;
    while(true){
        angle += 0.01f;
        float aspect = (float)mode.hdisplay / (float)mode.vdisplay;
        float proj[16]; memset(proj,0,sizeof(proj));
        float f = 1.0f / tanf(45.0f * 0.5f * M_PI / 180.0f);
        proj[0] = f / aspect; proj[5] = f; proj[10] = - (1000.0f + 0.1f) / (1000.0f - 0.1f); proj[11] = -1.0f; proj[14] = - (2.0f*1000.0f*0.1f) / (1000.0f - 0.1f);

        float mvp[16]; memcpy(mvp, proj, sizeof(mvp));

        glViewport(0,0, mode.hdisplay, mode.vdisplay);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, mvp);
        float lightDir[3] = { 0.5f, 0.7f, 0.3f };
        glUniform3fv(locLight, 1, lightDir);

        for(auto &m : meshes){
            glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
            if(locPos>=0){
                glEnableVertexAttribArray(locPos);
                glVertexAttribPointer(locPos, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (const void*)0);
            }
            if(locTex>=0){
                glEnableVertexAttribArray(locTex);
                glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (const void*)(sizeof(float)*3));
            }
            if(m.tex) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m.tex);
                glUniform1i(glGetUniformLocation(program, "uTex"), 0);
            }
            glDrawElements(GL_TRIANGLES, m.index_count, GL_UNSIGNED_SHORT, 0);
            if(locPos>=0) glDisableVertexAttribArray(locPos);
            if(locTex>=0) glDisableVertexAttribArray(locTex);
        }

        eglSwapBuffers(eglDisplay, eglSurface);
    }

    return 0;
}
