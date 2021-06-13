#pragma once
#include <grid.hpp>
#include <layer.hpp>
#include <vec3.hpp>

using Grid = Grid2D<LayerType>;

class Particle
{
public:
    // TODO: How soluble the particle is
    vec3f pos;

    Particle(vec3f p) : pos(p) {}

    // TODO: Add speed/momentum to movement?
    bool move(Grid &grid)
    {
        int x = (int)pos.x;
        int y = (int)pos.y;
        Grid::CellIterator it = grid.iterator(x, y);
        if (it == grid.end())
            return false;

        vec3f normal = grid.normal(it);

        // For the time being, we don't care about the z-axis, keep it consistent.
        normal.z = 0.0f;

        // Get the closest integer axis and extend the normal until the position
        // exits the current cell
        float nextX = (normal.x > 0) ? std::ceil(pos.x) : std::floor(pos.x);
        float nextY = (normal.y > 0) ? std::ceil(pos.y) : std::floor(pos.y);
        float factorX = std::fabs((pos.x - nextX) / normal.x);
        float factorY = std::fabs((pos.y - nextY) / normal.y);

        // Extend a little further so it crosses the boundary, otherwise the pos may think it's in the same cell.
        pos += normal * std::min(factorX, factorY) * 1.001f;

        return ((int)pos.x != x || (int)pos.y != y);
    }
};