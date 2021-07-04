#pragma once
#include <grid.hpp>
#include <layer.hpp>
#include <glm/glm.hpp>

using Grid = Grid2D<LayerType>;

const float DELTA = 0.01f;
const float TRANSFER_RATE = 0.001f;
const float EVAPORATION_RATE = 0.001f;
const float FRICTION_RATE = 0.999f;
const float EQUILIBRIUM_FACTOR = 0.1f;
const float MINIMUM_VOLUME = 0.01f;
const float MOMENTUM_BLENDING = 0.1f;

class Particle
{
public:
    // TODO: How soluble the particle is
    glm::vec2 pos;
    glm::vec2 speed{0};
    float volume = 1.0f;
    float sediment = 0.0f;
    float density = 1.0f;

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

        if (factor == 0.0f)
            return false;

        // Extend a little further so it crosses the boundary, otherwise the pos may think it's in the same cell.
        pos += glm::vec2{normal * factor * 1.001f};

        // ---------------------------------------------------------------------
        /*
        Thoughts:

        Apply friction per iteration. Ie, if friction reduced by 10% each time, then it's .9*.9*.9 ...
            pow(friction, factor) * speed
        speed *= std::pow(FRICTION_RATE, factor);
        
        1. Apply friction to slow
        2. Apply normal based speed increase
        3. If too slow, stop
        4. 


        // Factor indicates how many iterations of the normal are needed to traverse
        // the cell. Use as a form of delta.
        factor *= DELTA;
        */

        // Many flaws in this implementation
        // TODO: These equations should be influenced by the factor
        speed *= FRICTION_RATE; // std::pow(FRICTION_RATE, factor);

        // Average the normal with the current speed weight to mock momentum
        // TODO: Quick tests have some slight improvments to smoothness but possibly less beneficial overall
        //       Needs better testing to determine whether it's worth it or not
        // glm::vec2 newDirection = (glm::vec2{normal.x, normal.y} + speed * MOMENTUM_BLENDING) * 0.5f;
        // speed += newDirection / (volume * density);
        speed += glm::vec2{normal.x, normal.y} / (volume * density);

        // Can absorb more the faster it's moving
        float equilibrium = std::max(0.0f, volume * glm::length(speed) * EQUILIBRIUM_FACTOR);
        float transferToParticle = volume * TRANSFER_RATE * (equilibrium - sediment); // * factor

        // Ensure the amount being transferred between grid and particle is valid
        float transferQuantity = (transferToParticle > 0) ? std::min(it->depth, transferToParticle) : std::min(sediment, transferToParticle);
        it->depth -= transferQuantity;
        sediment += transferQuantity;

        // Remove some volume
        volume -= EVAPORATION_RATE; // * factor;
        // ---------------------------------------------------------------------

        // Whether or not we left the original cell
        return ((int)pos.x != x || (int)pos.y != y); // && volume > MINIMUM_VOLUME; // Doesn't appear to add anything, just slows down
    }
};