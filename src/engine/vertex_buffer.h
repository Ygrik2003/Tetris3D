#pragma once
#include "core/types.h"

#include <iostream>

template <class vertex_type>
class vertex_buffer
{
public:
    vertex_buffer(const triangle<vertex_type>* tri, std::size_t n);
    vertex_buffer(const vertex_type* vert, std::size_t n);
    ~vertex_buffer();

    void     bind() const;
    uint32_t size() const;

private:
    uint32_t gl_handle{ 0 };
    uint32_t count{ 0 };
};
