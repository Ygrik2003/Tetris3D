#include "primitive.h"

primitive::primitive(vector3d start_pos)
    : physics(start_pos)
{
    set_scale(1, 1, 1);
    set_rotate(0, 0, 0);
    set_translate(0, 0, 0);
}

primitive::primitive()
    : primitive(vector3d(0, 0, 0))
{
}

void primitive::add_figure(const figure& fig)
{
    figures.push_back(fig);
}

void primitive::update()
{
    vector3d pos = get_position();
    set_translate(pos);
    for (figure& fig : figures)
    {
        fig.set_translate(pos);
    }
}
