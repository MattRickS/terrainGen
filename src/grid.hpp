#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <vec3.hpp>

template <typename T>
struct Cell
{
    std::unique_ptr<Cell> below = nullptr;
    float depth = 0.0f;
    T value;

    Cell() {}
    Cell(float d, T val) : depth(d), value(val) {}

    float totalDepth()
    {
        float d{depth};
        if (below != nullptr)
            d += below->totalDepth();
        return d;
    }
    size_t numLayers()
    {
        size_t count{1};
        if (below != nullptr)
            count += below->numLayers();
        return count;
    }
};

template <typename T>
class Grid2D
{
public:
    std::vector<std::unique_ptr<Cell<T>>> values;

    Grid2D(size_t w, size_t h) : m_width(w), m_height(h), values(w * h) {}

    void fill(float depth, T val)
    {
        for (size_t y = 0; y < m_height; ++y)
            for (size_t x = 0; x < m_height; ++x)
                addDepth(x, y, depth, val);
    }
    void addDepth(size_t x, size_t y, float depth, T val)
    {
        if (depth <= 0.0f)
            return;

        auto origCell = cell(x, y);
        if (origCell == nullptr)
        {
            values[y * m_width + x] = std::make_unique<Cell<T>>(depth, val);
        }
        else if (origCell->depth <= 0)
        {
            origCell->value = val;
            origCell->depth = depth;
        }
        else
        {
            auto newCell = std::make_unique<Cell<T>>(depth, val);
            newCell->below = std::move(values[y * m_width + x]);
            values[y * m_width + x] = std::move(newCell);
        }
    }
    const size_t width() const { return m_height; };
    const size_t height() const { return m_width; };
    const size_t size() const { return m_width * m_height; };

    Cell<T> *deepestCell() const
    {
        auto it = std::max_element(values.begin(), values.end(), [](const auto &lhs, const auto &rhs) -> bool
                                   { lhs->totalDepth() < rhs->totalDepth(); });
        return (*it).get();
    }

    Cell<T> *mostLayeredCell() const
    {
        auto it = std::max_element(values.begin(), values.end(), [](const auto &lhs, const auto &rhs) -> bool
                                   { lhs->numLayers() < rhs->numLayers(); });
        return (*it).get();
    }

    template <typename U>
    bool isValidPos(const U x, const U y)
    {
        return (0 <= x && x < m_width && 0 <= y && y < m_height);
    }
    Cell<T> *cell(size_t x, size_t y) { return values[y * m_width + x].get(); }

private:
    size_t m_width, m_height;
};
