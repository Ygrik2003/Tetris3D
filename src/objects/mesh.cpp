#include "mesh.h"

mesh::mesh(std::vector<vertex3d_textured> vertexes,
           std::vector<uint16_t>          indexes)
{
    set_translate(0, 0, 0);
    set_rotate(0, 0, 0);
    set_scale(1, 1, 1);

    this->vertexes = vertexes;
    this->indexes  = indexes;

    count = indexes.size() / 3;
}