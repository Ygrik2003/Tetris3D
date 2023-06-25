#include "vertex_buffer.h"
#include "glad/glad.h"

template <class vertex_type>
vertex_buffer<vertex_type>::vertex_buffer(const triangle<vertex_type>* tri,
                                          std::size_t                  n)
    : count(static_cast<uint32_t>(n * 3))
{
    glGenBuffers(1, &gl_handle);
    GL_CHECK_ERRORS()

    bind();

    GLsizeiptr size_in_bytes =
        static_cast<GLsizeiptr>(n * 3 * sizeof(vertex_type));

    glBufferData(
        GL_ARRAY_BUFFER, size_in_bytes, tri->vertexes.data(), GL_STATIC_DRAW);
    GL_CHECK_ERRORS()
}

template <class vertex_type>
vertex_buffer<vertex_type>::vertex_buffer(const vertex_type* vert,
                                          std::size_t        n)
    : count(n)
{
    glGenBuffers(1, &gl_handle);
    GL_CHECK_ERRORS()

    bind();

    GLsizeiptr size_in_bytes = static_cast<GLsizeiptr>(n * sizeof(vertex_type));

    glBufferData(GL_ARRAY_BUFFER, size_in_bytes, vert, GL_STATIC_DRAW);
    GL_CHECK_ERRORS()
}

template <class vertex_type>
vertex_buffer<vertex_type>::~vertex_buffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK_ERRORS()
    glDeleteBuffers(1, &gl_handle);
    GL_CHECK_ERRORS()
}

template <class vertex_type>
void vertex_buffer<vertex_type>::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, gl_handle);
    GL_CHECK_ERRORS()
}

template <class vertex_type>
uint32_t vertex_buffer<vertex_type>::size() const
{
    return count;
}

template class vertex_buffer<vertex3d>;
template class vertex_buffer<vertex3d_colored>;
template class vertex_buffer<vertex3d_textured>;
template class vertex_buffer<vertex3d_colored_textured>;
template class vertex_buffer<vertex2d_colored_textured>;