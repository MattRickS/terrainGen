#include <iostream>
#include <fstream>
#include <random>

#include <FastNoiseLite.h>

#include <grid.hpp>
#include <layer.hpp>
#include <particle.hpp>
#include <ppm.hpp>

vec3f cellColor(Grid::Cell *cell)
{
    return cell->totalDepth();
    // vec3f color(0.0f);
    // while (cell != nullptr)
    // {
    //     color += LAYER_PROPERTIES[cell->value].color * cell->depth;
    //     cell = cell->below.get();
    // }
    // return color;
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

void writeNormals(std::ostream &out, Grid &grid)
{
    startPPM(out, grid.width(), grid.height());
    for (auto it = grid.begin(); it != grid.end(); ++it)
        writeColor(out, grid.normal(it) * 0.5f + 0.5f);
}

void writeGrid(std::ostream &out, Grid &grid)
{
    startPPM(out, grid.width(), grid.height());
    std::for_each(grid.begin(), grid.end(), [&](auto &cell)
                  { writeColor(out, cellColor(&cell)); });
}

void addNoiseLayer(FastNoiseLite &noise, Grid &grid, LayerType type, float mult = 1.0f)
{
    for (int y = 0; y < grid.height(); y++)
        for (int x = 0; x < grid.width(); x++)
            grid.addDepth(grid.iterator(x, y), (noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f) * mult, type);
}

void writeNormalsToFile(const char *name, Grid &grid)
{
    std::ofstream file(name);
    writeNormals(file, grid);
    file.close();
}

void writeGridToFile(const char *name, Grid &grid)
{
    std::ofstream file(name);
    writeGrid(file, grid);
    file.close();
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

void playParticles(Grid &grid)
{
    Grid particlePathGrid(grid.width(), grid.height());

    int particleIterations = 1000;
    int numParticles = 20000;

    std::random_device rd;                                   // only used once to initialise (seed) engine
    std::mt19937 rng(rd());                                  // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, grid.width()); // guaranteed unbiased

    std::cout << "Running " << numParticles << " particles" << std::endl;
    while (numParticles > 0)
    {
        playParticle(grid, particlePathGrid, uni(rng), uni(rng), particleIterations);
        numParticles--;
    }

    writeGridToFile("./images/particles.ppm", particlePathGrid);
}

int main(int argc, char const *argv[])
{
    Grid grid(512, 512);

    // Write first layer
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFrequency(0.005f);
    addNoiseLayer(noise, grid, LayerType::Rock, 3.0f);

    // Write second layer on top of first
    // noise.SetSeed(1783986);
    // addNoiseLayer(noise, grid, LayerType::Soil);

    // Remove an arbitrary section
    // for (int y = 100; y < 200; y++)
    //     for (int x = 100; x < 200; x++)
    //         grid.removeDepth(grid.iterator(x, y), 0.5f);

    // auto it = grid.begin() + 50;
    // std::cout << (it >= grid.end()) << std::endl;
    // std::cout << std::distance(grid.begin(), it) << std::endl;

    writeGridToFile("./images/height.ppm", grid);
    writeNormalsToFile("./images/normals.ppm", grid);

    playParticles(grid);

    return 0;
}
