#include "figure.h"

class primitive : object, physics
{
public:
    primitive(vector3d start_pos);
    primitive();

    void add_figure(const figure& fig);

    auto begin() const { return figures.begin(); };
    auto end() const { return figures.end(); };

    void update();

private:
    std::vector<figure> figures;
};