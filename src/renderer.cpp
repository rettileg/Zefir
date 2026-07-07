#include "renderer.h"
#include "shaderpack.h"
#include <glad/glad.h>
#include <cstdlib>
#include <cstring>

static unsigned int create_shader_from_strings(const char* vert_src, const char* frag_src) {
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert_src, nullptr);
    glCompileShader(vs);
    
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag_src, nullptr);
    glCompileShader(fs);
    
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return prog;
}

struct Renderer {
    unsigned int quad_vao, quad_vbo;
    unsigned int quad_shader;
    
    unsigned int ui_vao, ui_vbo, ui_ebo;
    unsigned int ui_shader;
    
    float projection[16];
    int width, height;
};

static float quad_vertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f
};

static float ui_vertices[] = {
    0.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 1.0f,  0.0f, 1.0f,
    1.0f, 0.0f,  1.0f, 0.0f,
    1.0f, 1.0f,  1.0f, 1.0f
};

static unsigned int ui_indices[] = {
    0, 1, 2, 1, 2, 3
};

Renderer* renderer_create() {
    Renderer* r = new Renderer();
    memset(r, 0, sizeof(Renderer));
    
    glGenVertexArrays(1, &r->quad_vao);
    glGenBuffers(1, &r->quad_vbo);
    
    glBindVertexArray(r->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glGenVertexArrays(1, &r->ui_vao);
    glGenBuffers(1, &r->ui_vbo);
    glGenBuffers(1, &r->ui_ebo);
    
    glBindVertexArray(r->ui_vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->ui_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ui_vertices), ui_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ui_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ui_indices), ui_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    r->quad_shader = create_shader_from_strings(quad_vert_src, quad_frag_src);
    r->ui_shader = create_shader_from_strings(ui_vert_src, ui_frag_src);
    
    return r;
}

void renderer_destroy(Renderer* r) {
    if (r) {
        glDeleteVertexArrays(1, &r->quad_vao);
        glDeleteBuffers(1, &r->quad_vbo);
        glDeleteProgram(r->quad_shader);
        
        glDeleteVertexArrays(1, &r->ui_vao);
        glDeleteBuffers(1, &r->ui_vbo);
        glDeleteBuffers(1, &r->ui_ebo);
        glDeleteProgram(r->ui_shader);
        
        delete r;
    }
}

void renderer_set_projection(Renderer* r, int width, int height) {
    r->width = width;
    r->height = height;
    
    float proj[16] = {
        2.0f / width, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / height, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    memcpy(r->projection, proj, sizeof(proj));
}

void renderer_clear(Renderer* r, float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (r->quad_shader) {
        glUseProgram(r->quad_shader);
        int loc = glGetUniformLocation(r->quad_shader, "uColor");
        glUniform4f(loc, red, green, blue, alpha);
        glBindVertexArray(r->quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void renderer_draw_rect_ex(Renderer* r, float x, float y, float w, float h,
                           float red, float green, float blue, float alpha,
                           float radius, int style,
                           float border_width,
                           const float border_color[4],
                           const float highlight_color[4],
                           const float shadow_color[4]) {
                            
    if (!r->ui_shader) return;
    
    glUseProgram(r->ui_shader);
    
    float model[16] = {
        w, 0.0f, 0.0f, 0.0f,
        0.0f, h, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, 0.0f, 1.0f
    };
    
    glUniformMatrix4fv(glGetUniformLocation(r->ui_shader, "uProjection"), 1, GL_FALSE, r->projection);
    glUniformMatrix4fv(glGetUniformLocation(r->ui_shader, "uModel"), 1, GL_FALSE, model);
    glUniform2f(glGetUniformLocation(r->ui_shader, "uSize"), w, h);
    glUniform1f(glGetUniformLocation(r->ui_shader, "uRadius"), radius);
    glUniform1i(glGetUniformLocation(r->ui_shader, "uStyle"), style);
    glUniform1f(glGetUniformLocation(r->ui_shader, "uBorderWidth"), border_width);
    
    glUniform4f(glGetUniformLocation(r->ui_shader, "uColor"), red, green, blue, alpha);
    glUniform4fv(glGetUniformLocation(r->ui_shader, "uBorderColor"), 1, border_color);
    glUniform4fv(glGetUniformLocation(r->ui_shader, "uHighlightColor"), 1, highlight_color);
    glUniform4fv(glGetUniformLocation(r->ui_shader, "uShadowColor"), 1, shadow_color);
    
    glBindVertexArray(r->ui_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderer_draw_rect(Renderer* r, float x, float y, float w, float h,
                        float red, float green, float blue, float alpha,
                        float radius, int style) {
    float border[4] = {0.70f, 0.67f, 0.63f, 1.0f};
    float highlight[4] = {1.0f, 1.0f, 1.0f, 0.95f};
    float shadow[4] = {0.50f, 0.47f, 0.43f, 0.9f};
    
    renderer_draw_rect_ex(r, x, y, w, h, red, green, blue, alpha, radius, style,
                          1.5f, border, highlight, shadow);
}