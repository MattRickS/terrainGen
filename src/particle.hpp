#pragma once
#include <grid.hpp>
#include <layer.hpp>
#include <vec3.hpp>

using Grid = Grid2D<LayerType>;

class Particle
{
public:
    vec3f pos;

    Particle(vec3f p) : pos(p) {}

    bool move(Grid &grid)
    {
        Grid::CellIterator it = grid.iterator((int)pos.x, (int)pos.y);
        if (it == grid.end())
            return false;
        vec3f normal = grid.normal(it);
        pos.x += normal.x;
        pos.y += normal.y;
        return true;
    }
};