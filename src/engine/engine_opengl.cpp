#include "engine_opengl.h"

#include "audio_buffer.h"
#include "objects/mesh.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <stdexcept>

bool ImGui_ImplSdlGL3_Init(SDL_Window* window, config& cfg);
void ImGui_ImplSdlGL3_Shutdown();
void ImGui_ImplSdlGL3_InvalidateDeviceObjects();
bool ImGui_ImplSdlGL3_CreateDeviceObjects(config& cfg);
void ImGui_ImplSdlGL3_NewFrame(SDL_Window* window);
bool ImGui_ImplSdlGL3_ProcessEvent(const SDL_Event* event);
void ImGui_ImplSdlGL3_RenderDrawLists(engine* eng, ImDrawData* draw_data);

static const char* get_sound_format_name(uint16_t format_value)
{
    static const std::map<uint16_t, const char*> format = {
        { SDL_AUDIO_U8, "AUDIO_U8" },
        { SDL_AUDIO_S8, "AUDIO_S8" },
        { SDL_AUDIO_S16LSB, "AUDIO_S16LSB" },
        { SDL_AUDIO_S16MSB, "AUDIO_S16MSB" },
        { SDL_AUDIO_S32LSB, "AUDIO_S32LSB" },
        { SDL_AUDIO_S32MSB, "AUDIO_S32MSB" },
        { SDL_AUDIO_F32LSB, "AUDIO_F32LSB" },
        { SDL_AUDIO_F32MSB, "AUDIO_F32MSB" },
    };

    auto it = format.find(format_value);
    return it->second;
}

#ifdef USE_GL_DEBUG
void APIENTRY gl_debug_output(GLenum        source,
                              GLenum        type,
                              GLuint        id,
                              GLenum        severity,
                              GLsizei       length,
                              const GLchar* message,
                              const void*   userParam)
{

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}
#endif

template <class vertex_type>
void bind_vertexes()
{
    glEnableVertexAttribArray(0);
    GL_CHECK_ERRORS()
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex_type),
        reinterpret_cast<GLvoid*>(vertex_type::OFFSET_POSITION));
    GL_CHECK_ERRORS()
}

template <class vertex_type>
void bind_normal()
{
    glEnableVertexAttribArray(1);
    GL_CHECK_ERRORS()
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex_type),
        reinterpret_cast<GLvoid*>(vertex_type::OFFSET_NORMAL));
    GL_CHECK_ERRORS()
}

template <class vertex_type>
void bind_texture_coords()
{
    glEnableVertexAttribArray(2);
    GL_CHECK_ERRORS()
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vertex_type),
        reinterpret_cast<GLvoid*>(vertex_type::OFFSET_TEXTURE));
    GL_CHECK_ERRORS()
}

template <class vertex_type>
void bind_colors()
{
    glEnableVertexAttribArray(3);
    GL_CHECK_ERRORS()
    glVertexAttribPointer(3,
                          4,
                          GL_UNSIGNED_BYTE,
                          GL_TRUE,
                          sizeof(vertex_type),
                          reinterpret_cast<GLvoid*>(vertex_type::OFFSET_COLOR));
    GL_CHECK_ERRORS()
}

void* load_gl_func(const char* name)
{
    SDL_FunctionPointer gl_pointer = SDL_GL_GetProcAddress(name);
    if (gl_pointer == nullptr)
    {
        std::cout << "can't load GL function: " << name << std::endl;
    }
    return reinterpret_cast<void*>(gl_pointer);
}

static float          g_Time            = 0.0;
static bool           g_MousePressed[3] = { false, false, false };
static float          g_MouseWheel      = 0.0f;
static shader_opengl* g_imgui_shader    = nullptr;

void ImGui_ImplSdlGL3_RenderDrawLists(engine* eng, ImDrawData* draw_data)
{
    ImGuiIO& io        = ImGui::GetIO();
    int      fb_width  = int(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int      fb_height = int(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
    {
        return;
    }
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    texture* texture = reinterpret_cast<texture_opengl*>(io.Fonts->TexID);
    g_imgui_shader->use();

    unsigned int texture_unit = 0;
    texture->bind();

    try
    {
        g_imgui_shader->set_uniform1("u_texture",
                                     static_cast<int>(0 + texture_unit));
        g_imgui_shader->set_uniform1("width",
                                     static_cast<float>(io.DisplaySize.x));
        g_imgui_shader->set_uniform1("height",
                                     static_cast<float>(io.DisplaySize.y));
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }

    glDisable(GL_DEPTH_TEST);
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list          = draw_data->CmdLists[n];
        const ImDrawIdx*  idx_buffer_offset = nullptr;

        const vertex2d_colored_textured* vertex_data =
            reinterpret_cast<const vertex2d_colored_textured*>(
                cmd_list->VtxBuffer.Data);
        size_t vert_count = static_cast<size_t>(cmd_list->VtxBuffer.size());

        vertex_buffer<vertex2d_colored_textured>* vertex_buff =
            new vertex_buffer(vertex_data, vert_count);

        const std::uint16_t* indexes = cmd_list->IdxBuffer.Data;
        size_t index_count = static_cast<size_t>(cmd_list->IdxBuffer.size());

        index_buffer* index_buff = new index_buffer(indexes, index_count);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

            texture_opengl* tex =
                reinterpret_cast<texture_opengl*>(pcmd->TextureId);
            eng->render_triangles(vertex_buff,
                                  index_buff,
                                  tex,
                                  idx_buffer_offset,
                                  pcmd->ElemCount);

            idx_buffer_offset += pcmd->ElemCount;
        } // end for cmd_i
        delete vertex_buff;
        delete index_buff;
    } // end for n
    glEnable(GL_DEPTH_TEST);
}

static const char* ImGui_ImplSdlGL3_GetClipboardText(void*)
{
    return SDL_GetClipboardText();
}

static void ImGui_ImplSdlGL3_SetClipboardText(void*, const char* text)
{
    SDL_SetClipboardText(text);
}

int engine_opengl::initialize(config& cfg)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) > 0)
    {
        throw std::runtime_error(std::string("Error in Init SDL3: ") +
                                 SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    if (cfg.is_full_sreen)
    {
        window = static_cast<SDL_Window*>(
            SDL_CreateWindow(cfg.app_name,
                             cfg.width,
                             cfg.height,
                             SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL));
    }
    else
    {
        window = static_cast<SDL_Window*>(SDL_CreateWindow(
            cfg.app_name, cfg.width, cfg.height, SDL_WINDOW_OPENGL));
    }
    int w, h;
    SDL_GetWindowSize(static_cast<SDL_Window*>(window), &w, &h);
    cfg.width  = w;
    cfg.height = h;

    _config = cfg;

    if (window == nullptr)
    {
        std::cerr << SDL_GetError();
        SDL_Quit();
        return 0;
    }
    audio_device_spec.freq     = 48000;
    audio_device_spec.format   = SDL_AUDIO_S16LSB;
    audio_device_spec.channels = 2;
    audio_device_spec.samples  = 1024; // must be power of 2
    audio_device_spec.callback = engine_opengl::audio_callback;
    audio_device_spec.userdata = this;

    const int num_audio_drivers = SDL_GetNumAudioDrivers();
    // for (int i = 0; i < num_audio_drivers; ++i)
    // {
    //     std::cout << "audio_driver #:" << i << " " << SDL_GetAudioDriver(i)
    //               << '\n';
    // }
    // std::cout << std::flush;

    const char* default_audio_device_name = nullptr;

    // SDL_FALSE - mean get only OUTPUT audio devices
    const int num_audio_devices = SDL_GetNumAudioDevices(SDL_FALSE);
    if (num_audio_devices > 0)
    {
        default_audio_device_name =
            SDL_GetAudioDeviceName(num_audio_devices - 1, SDL_FALSE);
        for (int i = 0; i < num_audio_devices; ++i)
        {
            std::cout << "audio device #" << i << ": "
                      << SDL_GetAudioDeviceName(i, SDL_FALSE) << '\n';
        }
    }
    std::cout << std::flush;

    audio_device = SDL_OpenAudioDevice(
        default_audio_device_name, 0, &audio_device_spec, nullptr, 0);

    if (audio_device == 0)
    {
        std::cerr << "failed open audio device: " << SDL_GetError();
        throw std::runtime_error("audio failed");
    }
    else
    {
        std::cout << "--------------------------------------------\n";
        std::cout << "audio device selected: " << default_audio_device_name
                  << '\n'
                  << "freq: " << audio_device_spec.freq << '\n'
                  << "format: "
                  << get_sound_format_name(audio_device_spec.format) << '\n'
                  << "channels: "
                  << static_cast<uint32_t>(audio_device_spec.channels) << '\n'
                  << "samples: " << audio_device_spec.samples << '\n'
                  << std::flush;

        SDL_PlayAudioDevice(audio_device);
    }

    int gl_major_v = 2;
    int gl_minor_v = 1;

    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
    // SDL_GL_CONTEXT_PROFILE_ES);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_v);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_v);

    gl_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(window));

    if (gl_context == nullptr)
    {
        std::cerr << SDL_GetError();
        SDL_Quit();
        return 0;
    }

    int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_v);
    assert(result == 0);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_v);
    assert(result == 0);

    std::clog << "OpenGL " << gl_major_v << '.' << gl_minor_v << '\n';

    if (gladLoadGLES2Loader(load_gl_func) == 0)
    {
        std::cerr << "cant init glad" << std::endl;
        SDL_Quit();
        return 0;
    }

#ifdef USE_GL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_output, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    GL_CHECK_ERRORS()
#endif

    GLuint vertex_buff = 0;
    glGenBuffers(1, &vertex_buff);
    GL_CHECK_ERRORS()
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buff);
    GL_CHECK_ERRORS()
    GLuint vertex_array_object = 0;
    glGenVertexArrays(1, &vertex_array_object);
    GL_CHECK_ERRORS()
    glBindVertexArray(vertex_array_object);
    GL_CHECK_ERRORS()

    glEnable(GL_BLEND);
    GL_CHECK_ERRORS()
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_CHECK_ERRORS()

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if (!ImGui_ImplSdlGL3_Init(static_cast<SDL_Window*>(window), _config))
    {
        throw std::runtime_error("error: failed to init ImGui");
    }

    ImGui_ImplSdlGL3_NewFrame(static_cast<SDL_Window*>(window));

    return 1;
}

void engine_opengl::uninitialize()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(static_cast<SDL_Window*>(window));
    SDL_Quit();
}

bool engine_opengl::event_keyboard(event& e)
{
    bool      is_event = false;
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event))
    {
        ImGui_ImplSdlGL3_ProcessEvent(&sdl_event);
        if (sdl_event.type == SDL_EVENT_QUIT)
        {
            e.action.quit = true;
            is_event      = true;
        }
        else if (sdl_event.type == SDL_EVENT_KEY_DOWN)
        {
            // clang-format off
            if (sdl_event.key.keysym.sym == SDLK_w) e.keyboard.w_clicked         = 1;
            if (sdl_event.key.keysym.sym == SDLK_s) e.keyboard.s_clicked         = 1;
            if (sdl_event.key.keysym.sym == SDLK_a) e.keyboard.a_clicked         = 1;
            if (sdl_event.key.keysym.sym == SDLK_d) e.keyboard.d_clicked         = 1;
            if (sdl_event.key.keysym.sym == SDLK_SPACE) e.keyboard.space_clicked = 1;
            // clang-format on
            is_event = true;
        }
        else if (sdl_event.type == SDL_EVENT_KEY_UP)
        {
            // clang-format off
            if (sdl_event.key.keysym.sym == SDLK_w) e.keyboard.w_released         = 1;
            if (sdl_event.key.keysym.sym == SDLK_s) e.keyboard.s_released         = 1;
            if (sdl_event.key.keysym.sym == SDLK_a) e.keyboard.a_released         = 1;
            if (sdl_event.key.keysym.sym == SDLK_d) e.keyboard.d_released         = 1;
            if (sdl_event.key.keysym.sym == SDLK_SPACE) e.keyboard.space_released = 1;
            // clang-format on
            is_event = true;
        }
        else if (sdl_event.type == SDL_EVENT_MOUSE_MOTION)
        {
            e.motion.x = sdl_event.motion.xrel;
            e.motion.y = sdl_event.motion.yrel;
            is_event   = true;
        }
        else if (sdl_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            // clang-format off
            if (sdl_event.button.button == SDL_BUTTON_LEFT) e.mouse.left_cliked = 1;
            // clang-format on
            is_event = true;
        }
        else if (sdl_event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            // clang-format off
            if (sdl_event.button.button == SDL_BUTTON_LEFT) e.mouse.left_released = 1;
            // clang-format on
            is_event = true;
        }
    }
    return is_event;
}

void engine_opengl::render_triangle(const triangle<vertex3d>& tr)
{
    reload_uniform();

    glBufferData(GL_ARRAY_BUFFER, sizeof(tr), &tr, GL_STATIC_DRAW);
    GL_CHECK_ERRORS()

    bind_vertexes<vertex3d>();
    bind_normal<vertex3d>();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void engine_opengl::render_triangle(const triangle<vertex3d_colored>& tr)
{
    reload_uniform();

    glBufferData(GL_ARRAY_BUFFER, sizeof(tr), &tr, GL_STATIC_DRAW);
    GL_CHECK_ERRORS()

    bind_vertexes<vertex3d_colored>();
    bind_normal<vertex3d_colored>();
    bind_colors<vertex3d_colored>();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void engine_opengl::render_triangle(const triangle<vertex3d_textured>& tr)
{
    reload_uniform();

    glBufferData(GL_ARRAY_BUFFER, sizeof(tr), &tr, GL_STATIC_DRAW);
    GL_CHECK_ERRORS()

    bind_vertexes<vertex3d_textured>();
    bind_normal<vertex3d_textured>();
    bind_texture_coords<vertex3d_textured>();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void engine_opengl::render_triangle(
    const triangle<vertex3d_colored_textured>& tr)
{
    reload_uniform();

    glBufferData(GL_ARRAY_BUFFER, sizeof(tr), &tr, GL_STATIC_DRAW);
    GL_CHECK_ERRORS()

    bind_vertexes<vertex3d_colored_textured>();
    bind_normal<vertex3d_colored_textured>();
    bind_texture_coords<vertex3d_colored_textured>();
    bind_colors<vertex3d_colored_textured>();

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void engine_opengl::render_triangles(vertex_buffer<vertex3d>* vertexes,
                                     index_buffer*            indexes,
                                     const std::uint16_t* start_vertex_index,
                                     size_t               num_vertexes)
{
    reload_uniform();

    vertexes->bind();
    indexes->bind();

    bind_vertexes<vertex3d>();
    bind_normal<vertex3d>();

    glDrawElements(
        GL_TRIANGLES, num_vertexes, GL_UNSIGNED_SHORT, start_vertex_index);

    GL_CHECK_ERRORS()
}
void engine_opengl::render_triangles(vertex_buffer<vertex3d_colored>* vertexes,
                                     index_buffer*                    indexes,
                                     const std::uint16_t* start_vertex_index,
                                     size_t               num_vertexes)
{
    reload_uniform();

    vertexes->bind();
    indexes->bind();

    bind_vertexes<vertex3d_colored>();
    bind_normal<vertex3d_colored>();
    bind_colors<vertex3d_colored>();

    glDrawElements(
        GL_TRIANGLES, num_vertexes, GL_UNSIGNED_SHORT, start_vertex_index);

    GL_CHECK_ERRORS()
}
void engine_opengl::render_triangles(vertex_buffer<vertex3d_textured>* vertexes,
                                     index_buffer*                     indexes,
                                     const texture*                    tex,
                                     const uint16_t* start_vertex_index,
                                     size_t          num_vertexes)
{
    reload_uniform();

    vertexes->bind();
    indexes->bind();
    tex->bind();

    bind_vertexes<vertex3d_textured>();
    bind_normal<vertex3d_textured>();
    bind_texture_coords<vertex3d_textured>();

    glDrawElements(
        GL_TRIANGLES, num_vertexes, GL_UNSIGNED_SHORT, start_vertex_index);

    GL_CHECK_ERRORS()
}

void engine_opengl::render_triangles(
    vertex_buffer<vertex3d_colored_textured>* vertexes,
    index_buffer*                             indexes,
    const texture*                            tex,
    const uint16_t*                           start_vertex_index,
    size_t                                    num_vertexes)
{
    vertexes->bind();
    indexes->bind();
    tex->bind();

    bind_vertexes<vertex3d_colored_textured>();
    bind_normal<vertex3d_colored_textured>();
    bind_texture_coords<vertex3d_colored_textured>();
    bind_colors<vertex3d_colored_textured>();

    glDrawElements(
        GL_TRIANGLES, num_vertexes, GL_UNSIGNED_SHORT, start_vertex_index);

    GL_CHECK_ERRORS()
}

void engine_opengl::render_triangles(
    vertex_buffer<vertex2d_colored_textured>* vertexes,
    index_buffer*                             indexes,
    const texture*                            tex,
    const uint16_t*                           start_vertex_index,
    size_t                                    num_vertexes)
{
    vertexes->bind();
    indexes->bind();
    tex->bind();

    bind_vertexes<vertex2d_colored_textured>();
    bind_texture_coords<vertex2d_colored_textured>();
    bind_colors<vertex2d_colored_textured>();

    glDrawElements(
        GL_TRIANGLES, num_vertexes, GL_UNSIGNED_SHORT, start_vertex_index);
    GL_CHECK_ERRORS()
}

void engine_opengl::swap_buffers()
{

    ImGui_ImplSdlGL3_RenderDrawLists(this, ImGui::GetDrawData());

    SDL_GL_SwapWindow(static_cast<SDL_Window*>(window));

    ImGui_ImplSdlGL3_NewFrame(static_cast<SDL_Window*>(window));

    glClearColor(77. / 255., 143. / 255., 210. / 255., 1.);
    GL_CHECK_ERRORS()

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GL_CHECK_ERRORS()
}

texture* engine_opengl::load_texture(uint32_t index, const char* path)
{
    // glActiveTexture(GL_TEXTURE0 + index);
    // GL_CHECK_ERRORS()
    // active_shader->set_uniform1("u_texture", index);

    texture* tex = new texture_opengl(path);
    tex->bind();

    return tex;
}

void engine_opengl::set_texture(uint32_t index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    active_shader->set_uniform1("u_texture", index);
}

void engine_opengl::set_uniform(uniform& uni)
{
    this->uniforms_world = &uni;
}

void engine_opengl::reload_uniform()
{
    try
    {
        active_shader->set_uniform2(
            "u_size_window", uniforms_world->width, uniforms_world->height);

        active_shader->set_uniform3("u_rotate_obj",
                                    *uniforms_world->rotate_alpha_obj,
                                    *uniforms_world->rotate_beta_obj,
                                    *uniforms_world->rotate_gamma_obj);

        active_shader->set_uniform3("u_rotate_camera",
                                    *uniforms_world->rotate_alpha_camera,
                                    *uniforms_world->rotate_beta_camera,
                                    *uniforms_world->rotate_gamma_camera);

        active_shader->set_uniform3("u_translate_obj",
                                    *uniforms_world->translate_x_obj,
                                    *uniforms_world->translate_y_obj,
                                    *uniforms_world->translate_z_obj);

        active_shader->set_uniform3("u_translate_camera",
                                    *uniforms_world->translate_x_camera,
                                    *uniforms_world->translate_y_camera,
                                    *uniforms_world->translate_z_camera);

        active_shader->set_uniform3("u_scale_obj",
                                    *uniforms_world->scale_x_obj,
                                    *uniforms_world->scale_y_obj,
                                    *uniforms_world->scale_z_obj);
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
}

std::mutex engine_opengl::audio_mutex;

void engine_opengl::audio_callback(void*    engine_ptr,
                                   uint8_t* stream,
                                   int      stream_size)
{

    std::lock_guard<std::mutex> lock(audio_mutex);

    std::fill_n(stream, stream_size, '\0');

    engine_opengl* e = static_cast<engine_opengl*>(engine_ptr);

    for (audio_buffer* buff : e->audio_output)
    {
        if (buff->is_playing)
        {
            uint32_t rest         = buff->length - buff->current_index;
            uint8_t* current_buff = &buff->buffer[buff->current_index];

            if (rest <= static_cast<uint32_t>(stream_size))
            {
                SDL_MixAudioFormat(stream,
                                   current_buff,
                                   e->audio_device_spec.format,
                                   rest,
                                   SDL_MIX_MAXVOLUME);
                buff->current_index += rest;
            }
            else
            {
                SDL_MixAudioFormat(stream,
                                   current_buff,
                                   e->audio_device_spec.format,
                                   static_cast<uint32_t>(stream_size),
                                   SDL_MIX_MAXVOLUME);
                buff->current_index += static_cast<uint32_t>(stream_size);
            }

            if (buff->current_index == buff->length)
            {
                if (buff->is_looped)
                {
                    buff->current_index = 0;
                }
                else
                {
                    buff->is_playing = false;
                }
            }
        }
    }
}

void engine_opengl::set_shader(shader* shader)
{
    this->active_shader = shader;

    active_shader->use();
}

void engine_opengl::set_relative_mouse_mode(bool state)
{
    if (state)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    else
        SDL_SetRelativeMouseMode(SDL_FALSE);
}

void engine_opengl::play_sound(const char* path)
{
    std::lock_guard<std::mutex> lock(audio_mutex);

    audio_buffer* audio_buff =
        new audio_buffer(path, audio_device, audio_device_spec);

    audio_buff->current_index = 0;
    audio_buff->is_playing    = true;
    audio_buff->is_looped     = false;

    audio_output.push_back(audio_buff);
}

bool ImGui_ImplSdlGL3_ProcessEvent(const SDL_Event* event)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type)
    {
        case SDL_EVENT_MOUSE_WHEEL:
        {
            if (event->wheel.y > 0)
                g_MouseWheel = 1;
            if (event->wheel.y < 0)
                g_MouseWheel = -1;
            return true;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event->button.button == SDL_BUTTON_LEFT)
                g_MousePressed[0] = true;
            if (event->button.button == SDL_BUTTON_RIGHT)
                g_MousePressed[1] = true;
            if (event->button.button == SDL_BUTTON_MIDDLE)
                g_MousePressed[2] = true;
            return true;
        }
        case SDL_EVENT_TEXT_INPUT:
        {
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            int key = event->key.keysym.scancode; // & ~SDLK_SCANCODE_MASK;
            io.KeysDown[key] = (event->type == SDL_EVENT_KEY_DOWN);
            io.KeyShift      = ((SDL_GetModState() & SDL_KMOD_SHIFT) != 0);
            io.KeyCtrl       = ((SDL_GetModState() & SDL_KMOD_CTRL) != 0);
            io.KeyAlt        = ((SDL_GetModState() & SDL_KMOD_ALT) != 0);
            io.KeySuper      = ((SDL_GetModState() & SDL_KMOD_GUI) != 0);
            return true;
        }
    }
    return false;
}

void ImGui_ImplSdlGL3_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO&       io     = ImGui::GetIO();
    unsigned char* pixels = nullptr;
    int            width  = 0;
    int            height = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Store our identifier
    io.Fonts->TexID = new texture_opengl(static_cast<void*>(pixels),
                                         static_cast<size_t>(width),
                                         static_cast<size_t>(height));
}

bool ImGui_ImplSdlGL3_CreateDeviceObjects(config& cfg)
{
    g_imgui_shader =
        new shader_opengl(cfg.shader_vertex_imgui, cfg.shader_fragment_imgui);

    ImGui_ImplSdlGL3_CreateFontsTexture();

    return true;
}

void ImGui_ImplSdlGL3_InvalidateDeviceObjects()
{
    void*           ptr     = ImGui::GetIO().Fonts->TexID;
    texture_opengl* texture = reinterpret_cast<texture_opengl*>(ptr);

    delete g_imgui_shader;
    g_imgui_shader = nullptr;
}

bool ImGui_ImplSdlGL3_Init(SDL_Window* window, config& cfg)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
    // Controls

    ImGui::StyleColorsDark();

    ImGuiIO& io            = ImGui::GetIO();
    io.BackendPlatformName = "imgui_menu";

    io.KeyMap[ImGuiKey_Tab]        = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]  = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]    = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow]  = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp]     = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown]   = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home]       = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End]        = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert]     = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete]     = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace]  = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space]      = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter]      = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape]     = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A]          = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C]          = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V]          = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X]          = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y]          = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z]          = SDL_SCANCODE_Z;
    /*
        g_MouseCursors[ImGuiMouseCursor_Arrow] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        g_MouseCursors[ImGuiMouseCursor_TextInput] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        g_MouseCursors[ImGuiMouseCursor_ResizeAll] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
        g_MouseCursors[ImGuiMouseCursor_ResizeNS] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        g_MouseCursors[ImGuiMouseCursor_ResizeEW] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        g_MouseCursors[ImGuiMouseCursor_ResizeNESW] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
        g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
        g_MouseCursors[ImGuiMouseCursor_Hand] =
            SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    */
    io.SetClipboardTextFn = ImGui_ImplSdlGL3_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplSdlGL3_GetClipboardText;
    io.ClipboardUserData  = nullptr;

    // #ifdef _WIN32
    //     // SDL_SysWMinfo wmInfo;
    //     // SDL_VERSION(&wmInfo.version);
    //     // SDL_GetWindowWMInfo(window, &wmInfo);
    //     // io.ImeWindowHandle = wmInfo.info.win.window;
    // #else
    //     (void)window;
    // #endif

    g_Time = SDL_GetTicks() / 1000.f;

    if (io.Fonts->TexID == nullptr)
    {
        ImGui_ImplSdlGL3_CreateDeviceObjects(cfg);
    }

    return true;
}

void ImGui_ImplSdlGL3_Shutdown()
{
    ImGui_ImplSdlGL3_InvalidateDeviceObjects();
    ImGui::DestroyContext();
}

void ImGui_ImplSdlGL3_NewFrame(SDL_Window* window)
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GetWindowSizeInPixels(window, &display_w, &display_h);
    io.DisplaySize             = ImVec2(float(w), float(h));
    io.DisplayFramebufferScale = ImVec2(w > 0 ? float(display_w / w) : 0.f,
                                        h > 0 ? float(display_h / h) : 0.f);

    // Setup time step
    Uint32 time         = SDL_GetTicks();
    float  current_time = time / 1000.0f;
    io.DeltaTime        = current_time - g_Time; // (1.0f / 60.0f);
    if (io.DeltaTime <= 0)
    {
        io.DeltaTime = 0.00001f;
    }
    g_Time = current_time;

    ImVec2   mouse_pos(-FLT_MAX, -FLT_MAX);
    uint32_t mouse_state = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
    // std::cout << mouse_pos.x << " " << mouse_pos.y << std::endl;
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
    {
        io.MousePos = mouse_pos;
    }
    else
    {
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    }

    io.MouseDown[0] = g_MousePressed[0] || (mouse_state & SDL_BUTTON_LEFT);
    io.MouseDown[1] = g_MousePressed[1];
    io.MouseDown[2] = g_MousePressed[2];

    std::fill_n(&g_MousePressed[0], 3, false);

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel  = 0.0f;

    if (io.MouseDrawCursor)
    {
        SDL_HideCursor();
    }
    else
    {
        SDL_ShowCursor();
    }
}
