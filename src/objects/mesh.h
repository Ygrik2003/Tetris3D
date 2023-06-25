#include "figure.h"

class mesh : public figure
{
public:
    mesh(std::vector<vertex3d_textured> vertexes,
         std::vector<uint16_t>          indexes);
};