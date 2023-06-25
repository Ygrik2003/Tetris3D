#include "core/types.h"

#include <cassert>
#include <iostream>

class index_buffer
{
public:
    index_buffer(const uint16_t* i, size_t n);
    ~index_buffer();
    void          bind() const;
    std::uint16_t size() const;

private:
    std::uint32_t gl_handle;
    std::uint16_t count;
};
