#include <iostream>
#include <FastNoiseLite.h>
#include <grid.hpp>
#include <layer.hpp>
#include <ppm.hpp>

using Grid = Grid2D<Layer>;

void writeGrid(std::ostream &out, Grid &grid)
{
    startPPM(std::cout, grid.width(), grid.height());
    for (auto &cell : grid.values)
        writeColor(std::cout, LAYER_PROPERTIES[cell.value.type].color * cell.value.height);
}

void fillGridNoise(FastNoiseLite &noise, Grid &grid, LayerType type)
{
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    for (int y = 0; y < grid.height(); y++)
    {
        for (int x = 0; x < grid.width(); x++)
        {
            auto &layer = grid.cell(x, y).value;
            layer.height = noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f;
            layer.type = type;
        }
    }
}

void addNoiseLayer(FastNoiseLite &noise, Grid &grid, LayerType type)
{
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    for (int y = 0; y < grid.height(); y++)
    {
        for (int x = 0; x < grid.width(); x++)
        {
            grid.addLayer(x, y, {type, grid.cell(x, y).value.height + noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f});
        }
    }
}

int main(int argc, char const *argv[])
{
    Grid grid(512, 512);
    FastNoiseLite noise;
    fillGridNoise(noise, grid, LayerType::Rock);
    FastNoiseLite noise2(1783986);
    addNoiseLayer(noise2, grid, LayerType::Soil);
    writeGrid(std::cout, grid);
    return 0;
}
