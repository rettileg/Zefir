#include "texture_rect.h"
#include <glad/glad.h>

static unsigned int g_shader = 0;
static unsigned int g_vao = 0, g_vbo = 0;
static bool g_initialized = false;

static void init() {
    if (g_initialized) return;
    
    const char* vs = R"(
#version 460 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTex;
out vec2 vTex;
uniform mat4 uProj;
uniform mat4 uModel;
void main() { gl_Position = uProj * uModel * vec4(aPos, 0.0, 1.0); vTex = aTex; }
)";
    
    const char* fs = R"(
#version 460 core
in vec2 vTex;
out vec4 FragColor;
uniform sampler2D uTex;
void main() { FragColor = texture(uTex, vTex); }
)";
    
    unsigned int vs_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_id, 1, &vs, nullptr); glCompileShader(vs_id);
    unsigned int fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_id, 1, &fs, nullptr); glCompileShader(fs_id);
    g_shader = glCreateProgram();
    glAttachShader(g_shader, vs_id); glAttachShader(g_shader, fs_id); glLinkProgram(g_shader);
    glDeleteShader(vs_id); glDeleteShader(fs_id);
    
    float verts[] = {0,0,0,0, 0,1,0,1, 1,1,1,1, 0,0,0,0, 1,1,1,1, 1,0,1,0};
    glGenVertexArrays(1, &g_vao); glBindVertexArray(g_vao);
    glGenBuffers(1, &g_vbo); glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float))); glEnableVertexAttribArray(1);
    
    g_initialized = true;
}

void zefir_texture_rect(ZefirContext* ctx, unsigned int texture_id, float x, float y, float w, float h) {
    if (!ctx || !texture_id) return;
    init();
    
    int ww, wh;
    zefir_get_size(ctx, &ww, &wh);
    
    float proj[16] = {2.0f/ww,0,0,0, 0,-2.0f/wh,0,0, 0,0,-1,0, -1,1,0,1};
    float model[16] = {w,0,0,0, 0,h,0,0, 0,0,1,0, x,y,0,1};
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(g_shader);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "uProj"), 1, GL_FALSE, proj);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "uModel"), 1, GL_FALSE, model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glBindVertexArray(g_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void zefir_texture_rect_ex(ZefirContext* ctx, unsigned int texture_id, float x, float y, float w, float h, float scale) {
    if (!ctx || !texture_id) return;
    init();
    
    int ww, wh;
    zefir_get_size(ctx, &ww, &wh);
    
    float sw = w * scale;
    float sh = h * scale;
    float sx = x + (w - sw) / 2.0f;
    float sy = y + (h - sh) / 2.0f;
    
    float proj[16] = {2.0f/ww,0,0,0, 0,-2.0f/wh,0,0, 0,0,-1,0, -1,1,0,1};
    float model[16] = {sw,0,0,0, 0,sh,0,0, 0,0,1,0, sx,sy,0,1};
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(g_shader);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "uProj"), 1, GL_FALSE, proj);
    glUniformMatrix4fv(glGetUniformLocation(g_shader, "uModel"), 1, GL_FALSE, model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glBindVertexArray(g_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}