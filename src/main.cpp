#include <iostream>

#include <FastNoiseLite.h>

#include <grid.hpp>
#include <layer.hpp>
#include <particle.hpp>
#include <ppm.hpp>

vec3f cellColor(Grid::Cell *cell)
{
    vec3f color(0.0f);
    while (cell != nullptr)
    {
        color += LAYER_PROPERTIES[cell->value].color * cell->depth;
        cell = cell->below.get();
    }
    return color;
}

void writeNormals(std::ostream &out, Grid &grid)
{
    startPPM(std::cout, grid.width(), grid.height());
    for (auto it = grid.begin(); it != grid.end(); ++it)
        writeColor(std::cout, grid.normal(it) * 0.5f + 0.5f);
}

void writeGrid(std::ostream &out, Grid &grid)
{
    startPPM(std::cout, grid.width(), grid.height());
    std::for_each(grid.begin(), grid.end(), [&](auto &cell)
                  { writeColor(std::cout, cellColor(&cell)); });
}

void addNoiseLayer(FastNoiseLite &noise, Grid &grid, LayerType type, float mult = 1.0f)
{
    for (int y = 0; y < grid.height(); y++)
        for (int x = 0; x < grid.width(); x++)
            grid.addDepth(grid.iterator(x, y), (noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f) * mult, type);
}

Grid::CellIterator deepestCell(Grid &grid)
{
    return std::max_element(grid.begin(), grid.end(), [](auto &lhs, auto &rhs) -> bool
                            { lhs.totalDepth() < rhs.totalDepth(); });
}
Grid::CellIterator mostLayeredCell(Grid &grid)
{
    return std::max_element(grid.begin(), grid.end(), [](auto &lhs, auto &rhs) -> bool
                            { lhs.numLayers() < rhs.numLayers(); });
}

void playParticle(Grid &grid, Grid &renderGrid, size_t x, size_t y, size_t n = 1000)
{
    vec3f pos{(float)x, (float)y, grid.iterator(x, y)->totalDepth()};
    Particle part(pos);

    while (n > 0 && part.move(grid))
    {
        auto it = renderGrid.iterator((int)part.pos.x, (int)part.pos.y);
        if (it != renderGrid.end())
            renderGrid.addDepth(it, 0.01f, LayerType::Air);
        n--;
    }
}

int main(int argc, char const *argv[])
{
    Grid grid(512, 512);

    // Write first layer
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.01f);
    addNoiseLayer(noise, grid, LayerType::Rock, 10.0f);

    // Write second layer on top of first
    // noise.SetSeed(1783986);
    // addNoiseLayer(noise, grid, LayerType::Soil);

    // Remove an arbitrary section
    // for (int y = 100; y < 200; y++)
    //     for (int x = 100; x < 200; x++)
    //         grid.removeDepth(grid.iterator(x, y), 0.5f);

    writeGrid(std::cout, grid);
    writeNormals(std::cout, grid);

    Grid particlePathGrid(grid.width(), grid.height());

    playParticle(grid, particlePathGrid, 200, 200);
    // for (auto it = grid.begin(); it != grid.end(); it += 100)
    // {
    //     int index = std::distance(grid.begin(), it);
    //     playParticle(grid, particlePathGrid, index % grid.width(), index / grid.width());
    // }

    writeGrid(std::cout, particlePathGrid);
    return 0;
}
