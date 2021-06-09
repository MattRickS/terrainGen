#include <iostream>
#include <FastNoiseLite.h>
#include <grid.hpp>
#include <layer.hpp>
#include <ppm.hpp>

using Grid = Grid2D<LayerType>;

vec3f cellColor(Cell<LayerType> *cell)
{
    vec3f color(0.0f);
    while (cell != nullptr)
    {
        color += LAYER_PROPERTIES[cell->value].color * cell->depth;
        cell = cell->below.get();
    }
    return color;
}

void writeGrid(std::ostream &out, Grid &grid)
{
    startPPM(std::cout, grid.width(), grid.height());
    for (auto &cell : grid.values)
        writeColor(std::cout, cellColor(cell.get()));
}

void addNoiseLayer(FastNoiseLite &noise, Grid &grid, LayerType type)
{
    for (int y = 0; y < grid.height(); y++)
    {
        for (int x = 0; x < grid.width(); x++)
        {
            grid.addDepth(x, y, noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f, type);
        }
    }
}

int main(int argc, char const *argv[])
{
    Grid grid(512, 512);

    // Write first layer
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    addNoiseLayer(noise, grid, LayerType::Rock);
    writeGrid(std::cout, grid);

    // Write second layer on top of first
    FastNoiseLite noise2(1783986);
    noise2.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    addNoiseLayer(noise2, grid, LayerType::Soil);
    writeGrid(std::cout, grid);

    return 0;
}
