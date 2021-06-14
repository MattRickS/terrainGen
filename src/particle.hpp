#pragma once
#include <grid.hpp>
#include <layer.hpp>
#include <glm/glm.hpp>

using Grid = Grid2D<LayerType>;

class Particle
{
public:
    // TODO: How soluble the particle is
    glm::vec2 pos;

    Particle(glm::vec2 p) : pos(p) {}

    // TODO: Add speed/momentum to movement?
    bool move(Grid &grid)
    {
        int x = (int)pos.x;
        int y = (int)pos.y;
        Grid::CellIterator it = grid.iterator(x, y);
        if (it == grid.end())
            return false;

        glm::vec3 normal = grid.normal(it);

        // Get the closest integer axis and extend the normal until the position
        // exits the current cell
        float nextX = (normal.x > 0) ? std::ceil(pos.x) : std::floor(pos.x);
        float nextY = (normal.y > 0) ? std::ceil(pos.y) : std::floor(pos.y);
        float factorX = std::fabs((pos.x - nextX) / normal.x);
        float factorY = std::fabs((pos.y - nextY) / normal.y);

        // Extend a little further so it crosses the boundary, otherwise the pos may think it's in the same cell.
        pos += glm::vec2{normal * std::min(factorX, factorY) * 1.001f};

        return ((int)pos.x != x || (int)pos.y != y);
    }
};