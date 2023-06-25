#include "audio_buffer.h"
#include "engine.h"
#include "imgui/imgui.h"
#include "texture.h"

#include <SDL3/SDL.h>

class engine_opengl : public engine
{
public:
    int  initialize(config& _config) override;
    void uninitialize() override;

    bool event_keyboard(event&) override;

    void render_triangle(const triangle<vertex3d>& tr) override;
    void render_triangle(const triangle<vertex3d_colored>& tr) override;
    void render_triangle(const triangle<vertex3d_textured>& tr) override;
    void render_triangle(
        const triangle<vertex3d_colored_textured>& tr) override;

    void render_triangles(vertex_buffer<vertex3d>* vertexes,
                          index_buffer*            indexes,
                          const std::uint16_t*     start_vertex_index,
                          size_t                   num_vertexes) override;
    void render_triangles(vertex_buffer<vertex3d_colored>* vertexes,
                          index_buffer*                    indexes,
                          const std::uint16_t*             start_vertex_index,
                          size_t num_vertexes) override;
    void render_triangles(vertex_buffer<vertex3d_textured>* vertexes,
                          index_buffer*                     indexes,
                          const texture*                    tex,
                          const uint16_t*                   start_vertex_index,
                          size_t num_vertexes) override;
    void render_triangles(vertex_buffer<vertex3d_colored_textured>* vertexes,
                          index_buffer*                             indexes,
                          const texture*                            tex,
                          const uint16_t* start_vertex_index,
                          size_t          num_vertexes) override;
    void render_triangles(vertex_buffer<vertex2d_colored_textured>* vertexes,
                          index_buffer*                             indexes,
                          const texture*                            tex,
                          const std::uint16_t* start_vertex_index,
                          size_t               num_vertexes) override;

    void swap_buffers() override;

    texture* load_texture(uint32_t index, const char* path) override;

    void set_texture(uint32_t index) override;
    void set_uniform(uniform& uni) override;
    void set_shader(shader* shader) override;
    void set_relative_mouse_mode(bool state) override;

    void play_sound(const char* path) override;

    void reload_uniform() override;

private:
    SDL_Window*   window         = nullptr;
    SDL_GLContext gl_context     = nullptr;
    shader*       active_shader  = nullptr;
    uniform*      uniforms_world = nullptr;

    SDL_AudioDeviceID          audio_device;
    SDL_AudioSpec              audio_device_spec;
    std::vector<audio_buffer*> audio_output;

    static void       audio_callback(void*    engine_ptr,
                                     uint8_t* stream,
                                     int      stream_size);
    static std::mutex audio_mutex;
};
