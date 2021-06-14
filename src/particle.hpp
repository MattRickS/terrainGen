#pragma once
#include <grid.hpp>
#include <layer.hpp>
#include <glm/glm.hpp>

using Grid = Grid2D<LayerType>;

const float DENSITY = 1.0f;
const float DEPOSITION_RATE = 0.01f;
const float EVAPORATION_RATE = 0.001f;
const float FRICTION_RATE = 0.95f;

class Particle
{
public:
    // TODO: How soluble the particle is
    glm::vec2 pos;
    glm::vec2 speed{0};
    float volume = 1.0f;
    float sediment = 0.0f;

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
        float factor = std::min(
            std::fabs((pos.x - nextX) / normal.x),
            std::fabs((pos.y - nextY) / normal.y));

        // Extend a little further so it crosses the boundary, otherwise the pos may think it's in the same cell.
        pos += glm::vec2{normal * factor * 1.001f};

        // ---------------------------------------------------------------------

        // Many flaws in this implementation
        // TODO: These equations should be influenced by the factor
        speed *= FRICTION_RATE;
        speed += glm::vec2{normal.x} / (volume * DENSITY);

        // Tie the equilibrium to the speed of the particle
        float equilibrium = std::max(0.0f, volume * glm::length(speed) * 0.1f);

        // Compute Capacity Difference ("Driving Force")
        float diff = equilibrium - sediment;

        // Perform the Mass Transfer!
        // TODO: Should check there is enough sediment to be removed first
        sediment = std::max(0.0f, sediment + DEPOSITION_RATE * diff);
        it->depth -= volume * DEPOSITION_RATE * diff;

        volume -= EVAPORATION_RATE;
        // ---------------------------------------------------------------------

        // Whether or not we left the original cell
        return ((int)pos.x != x || (int)pos.y != y);
    }
};