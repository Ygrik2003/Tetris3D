#include "shader_opengl.h"
#include "glad/glad.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

shader_opengl::shader_opengl(const char* path_to_vertex,
                             const char* path_to_fragment)
{
    this->path_to_vertex   = path_to_vertex;
    this->path_to_fragment = path_to_fragment;

    program = glCreateProgram();
    GL_CHECK_ERRORS()

    load(path_to_vertex, GL_VERTEX_SHADER);
    load(path_to_fragment, GL_FRAGMENT_SHADER);

    glLinkProgram(program);
    GL_CHECK_ERRORS()

    glBindAttribLocation(program, 0, "i_position");
    GL_CHECK_ERRORS()

    use();
}

shader_opengl::~shader_opengl()
{
    glDeleteProgram(program);
}

GLuint shader_opengl::get_program_id() const
{
    return program;
}

void shader_opengl::use() const
{
    glUseProgram(program);
    GL_CHECK_ERRORS();
}

void shader_opengl::reload()
{
    glDeleteProgram(program);
    GL_CHECK_ERRORS()

    program = glCreateProgram();
    GL_CHECK_ERRORS()

    load(path_to_vertex, GL_VERTEX_SHADER);
    load(path_to_fragment, GL_FRAGMENT_SHADER);

    glLinkProgram(program);
    GL_CHECK_ERRORS()

    use();
}

void shader_opengl::load(const char* path, int type)
{
    membuff*    file     = load_file_to_memory(path);
    const char* c_buffer = file->ptr.get();

    GLuint shader = glCreateShader(type);
    GL_CHECK_ERRORS()

    glShaderSource(shader, 1, &c_buffer, nullptr);
    GL_CHECK_ERRORS()

    glCompileShader(shader);
    GL_CHECK_ERRORS()

    GLint result = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    GL_CHECK_ERRORS()
    if (result == 0)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);
        GL_CHECK_ERRORS()

        std::vector<char> log(static_cast<size_t>(result));
        glGetShaderInfoLog(shader, result, nullptr, log.data());
        GL_CHECK_ERRORS()

        glDeleteShader(shader);
        GL_CHECK_ERRORS()

        delete file;

        throw std::runtime_error(log.data());
    }
    glAttachShader(program, shader);
    GL_CHECK_ERRORS()

    glDeleteShader(shader);
    GL_CHECK_ERRORS()

    delete file;
}

void shader_opengl::set_uniform1(const char* name, int value)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform1i(location, value);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform1(const char* name, uint32_t value)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform1ui(location, value);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform1(const char* name, float value)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform1f(location, value);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform2(const char* name, int val1, int val2)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform2i(location, val1, val2);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform2(const char* name, uint32_t val1, uint32_t val2)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform2ui(location, val1, val2);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform2(const char* name, float val1, float val2)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform2f(location, val1, val2);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform3(const char* name, int val1, int val2, int val3)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform3i(location, val1, val2, val3);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform3(const char* name,
                                 uint32_t    val1,
                                 uint32_t    val2,
                                 uint32_t    val3)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform3ui(location, val1, val2, val3);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform3(const char* name,
                                 float       val1,
                                 float       val2,
                                 float       val3)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform3f(location, val1, val2, val3);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform4(
    const char* name, int val1, int val2, int val3, int val4)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform4i(location, val1, val2, val3, val4);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform4(const char* name,
                                 uint32_t    val1,
                                 uint32_t    val2,
                                 uint32_t    val3,
                                 uint32_t    val4)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform4ui(location, val1, val2, val3, val4);
    GL_CHECK_ERRORS()
}

void shader_opengl::set_uniform4(
    const char* name, float val1, float val2, float val3, float val4)
{
    use();
    const int location = glGetUniformLocation(program, name);
    GL_CHECK_ERRORS()
    if (location == -1)
    {
        throw std::runtime_error(std::string("can't get uniform location: ") +
                                 name);
    }
    glUniform4f(location, val1, val2, val3, val4);
    GL_CHECK_ERRORS()
}
