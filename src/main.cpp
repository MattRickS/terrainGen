#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>
#include <random>

#include <FastNoiseLite.h>
#include <glm/glm.hpp>

#include <grid.hpp>
#include <layer.hpp>
#include <particle.hpp>
#include <ppm.hpp>

#define DEBUG

class Timer
{
public:
    std::chrono::system_clock::time_point timer;

    Timer()
    {
        timer = std::chrono::high_resolution_clock::now();
    }

    void printTimedMessage(const char *msg)
    {
        auto done = std::chrono::high_resolution_clock::now();
        std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(done - timer).count() << "ms] " << msg << std::endl;
        timer = done;
    }
};

Grid::CellIterator deepestCell(Grid &grid)
{
    return std::max_element(grid.begin(), grid.end(), [](auto &lhs, auto &rhs) -> bool
                            { return lhs.totalDepth() < rhs.totalDepth(); });
}

using GetColorFunc = typename std::function<glm::vec3(Grid::CellIterator)>;

void writeGrid(std::ostream &out, Grid &grid, const GetColorFunc &getColor)
{
    startPPM(out, grid.width(), grid.height());
    for (auto it = grid.begin(); it != grid.end(); ++it)
        writeColor(out, getColor(it));
}

void writeGridToFile(const char *name, Grid &grid, const GetColorFunc &getColor)
{
    std::ofstream file(name);
    writeGrid(file, grid, getColor);
    file.close();
}

glm::vec3 cellColor(Grid::CellIterator it)
{
    return glm::vec3{it->totalDepth()};
}

void addNoiseLayer(FastNoiseLite &noise, Grid &grid, LayerType type, float mult = 1.0f)
{
    for (int y = 0; y < grid.height(); y++)
        for (int x = 0; x < grid.width(); x++)
            grid.addDepth(grid.iterator(x, y), (noise.GetNoise((float)x, (float)y) * 0.5f + 0.5f) * mult, type);
}

void playParticle(Grid &grid, Grid &renderGrid, size_t x, size_t y, size_t n = 1000)
{
    // Start in the middle of the cell to avoid 0 distance to adjacent cell.
    glm::vec3 pos{x + 0.5f, y + 0.5f, grid.iterator(x, y)->totalDepth()};
    Particle part(pos);

    while (n > 0 && part.move(grid))
    {
        auto it = renderGrid.iterator((int)part.pos.x, (int)part.pos.y);
        if (it != renderGrid.end())
            renderGrid.addDepth(it, 0.02f, LayerType::Air);
        n--;
    }

    // Drop any remaining sediment
    auto it = grid.iterator((int)part.pos.x, (int)part.pos.y);
    if (it != grid.end())
        it->depth += part.sediment;
}

void playParticles(Grid &grid, Timer &timer)
{
    Grid particlePathGrid(grid.width(), grid.height());

    int particleIterations = 100;
    int numParticles = 4000000;
    int seed = 1337; // rd()

    std::random_device rd;                                   // only used once to initialise (seed) engine
    std::mt19937 rng(seed);                                  // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, grid.width()); // guaranteed unbiased

    while (numParticles > 0)
    {
        playParticle(grid, particlePathGrid, uni(rng), uni(rng), particleIterations);
        numParticles--;
    }

#ifdef DEBUG
    timer.printTimedMessage("Particles played");
    writeGridToFile("./images/particles.ppm", particlePathGrid, cellColor);
    timer.printTimedMessage("Particle image written");
#endif
}

int main(int argc, char const *argv[])
{
    Timer timer{};

    Grid grid(128, 128);

    // Write first layer
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFrequency(0.005f);
    addNoiseLayer(noise, grid, LayerType::Rock, 1.0f);

#ifdef DEBUG
    timer.printTimedMessage("Noise generated");
    float maxDepth = deepestCell(grid)->totalDepth();
    std::cout << "Normalising height map to max depth: " << maxDepth << std::endl;

    writeGridToFile("./images/height.ppm", grid, [&grid, &maxDepth](Grid::CellIterator it) -> glm::vec3
                    { return glm::vec3{it->totalDepth() / maxDepth}; });
    writeGridToFile("./images/normals.ppm", grid, [&grid](Grid::CellIterator it) -> glm::vec3
                    { return grid.normal(it) * 0.5f + 0.5f; });
    timer.printTimedMessage("Initial Images written");
#endif

    playParticles(grid, timer);

#ifdef DEBUG
    writeGridToFile("./images/erode_height.ppm", grid, [&grid, &maxDepth](Grid::CellIterator it) -> glm::vec3
                    { return glm::vec3{it->totalDepth() / maxDepth}; });
    writeGridToFile("./images/erode_normals.ppm", grid, [&grid](Grid::CellIterator it) -> glm::vec3
                    { return grid.normal(it) * 0.5f + 0.5f; });
    timer.printTimedMessage("Eroded images written");
#endif

    return 0;
}
