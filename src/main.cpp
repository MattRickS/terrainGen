#include <iostream>

#include <FastNoiseLite.h>

#include <grid.hpp>
#include <layer.hpp>
#include <ppm.hpp>

using Grid = Grid2D<LayerType>;

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
        writeColor(std::cout, grid.normal(it));
}

void writeGrid(std::ostream &out, Grid &grid)
{
    startPPM(std::cout, grid.width(), grid.height());
    std::for_each(grid.begin(), grid.end(), [&](auto &cell)
                  { writeColor(std::cout, cellColor(&cell)); });
}

void addNoiseLayer(FastNoiseLite &noise, Grid &grid, LayerType type)
{
    for (int y = 0; y < grid.height(); y++)
        for (int x = 0; x < grid.width(); x++)
            grid.addDepth(grid.iterator(x, y), noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f, type);
}

int main(int argc, char const *argv[])
{
    Grid grid(512, 512);

    // Write first layer
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.1f);
    addNoiseLayer(noise, grid, LayerType::Rock);

    // Write second layer on top of first
    noise.SetSeed(1783986);
    addNoiseLayer(noise, grid, LayerType::Soil);

    // Remove an arbitrary section
    for (int y = 100; y < 200; y++)
        for (int x = 100; x < 200; x++)
            grid.removeDepth(grid.iterator(x, y), 0.5f);

    writeGrid(std::cout, grid);
    writeNormals(std::cout, grid);

    // std::cout
    //     << "P3\nmax depth: " << grid.deepestCell()->totalDepth() << std::endl
    //     << "Max Layers: " << grid.mostLayeredCell()->numLayers() << std::endl;

    return 0;
}
