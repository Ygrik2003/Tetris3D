#pragma once
#include "core/config.h"
#include "core/event.h"
#include "core/types.h"
#include "index_buffer.h"
#include "objects/figure.h"
#include "shader_opengl.h"
#include "texture_opengl.h"
#include "vertex_buffer.h"

#include <iostream>
#include <vector>

class engine
{
public:
    virtual ~engine() = default;

    virtual int  initialize(config& _config) = 0;
    virtual void uninitialize()              = 0;

    virtual bool event_keyboard(event&) = 0;
    virtual void generate_shadow_map()  = 0;

    virtual void render_triangle(const triangle<vertex3d>& tr)          = 0;
    virtual void render_triangle(const triangle<vertex3d_colored>& tr)  = 0;
    virtual void render_triangle(const triangle<vertex3d_textured>& tr) = 0;
    virtual void render_triangle(
        const triangle<vertex3d_colored_textured>& tr) = 0;

    virtual void render_triangles(vertex_buffer<vertex3d>*,
                                  index_buffer*,
                                  const uint16_t*,
                                  size_t) = 0;
    virtual void render_triangles(vertex_buffer<vertex3d_colored>*,
                                  index_buffer*,
                                  const uint16_t*,
                                  size_t) = 0;
    virtual void render_triangles(vertex_buffer<vertex3d_textured>*,
                                  index_buffer*,
                                  const texture*,
                                  const uint16_t*,
                                  size_t) = 0;
    virtual void render_triangles(vertex_buffer<vertex3d_colored_textured>*,
                                  index_buffer*,
                                  const texture*,
                                  const uint16_t*,
                                  size_t) = 0;
    virtual void render_triangles(
        vertex_buffer<vertex2d_colored_textured>* vertexes,
        index_buffer*                             indexes,
        const texture*                            tex,
        const uint16_t*                           start_vertex_index,
        size_t                                    num_vertexes) = 0;

    virtual void swap_buffers() = 0;

    virtual void     reload_uniform()                               = 0;
    virtual texture* load_texture(uint32_t index, const char* path) = 0;

    virtual void set_texture(uint32_t index)         = 0;
    virtual void set_uniform(uniform& uni)           = 0;
    virtual void set_shader(shader* shader)          = 0;
    virtual void set_relative_mouse_mode(bool state) = 0;

    virtual void play_sound(const char* path, bool is_looped) = 0;

protected:
    unsigned int depthMapFBO;
    unsigned int depthMap;
    config _config;
};