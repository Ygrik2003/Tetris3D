#pragma once

class texture
{
public:
    virtual ~texture() = default;

    virtual void     bind() const       = 0;
    virtual uint32_t get_width() const  = 0;
    virtual uint32_t get_height() const = 0;
};