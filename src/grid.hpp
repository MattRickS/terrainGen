#pragma once
#include <memory>
#include <vector>
#include <vec3.hpp>

template <typename T>
struct Cell
{
    std::unique_ptr<Cell> below = nullptr;
    float depth;
    T value;

    Cell() {}
    Cell(float d, T val) : depth(d), value(val) {}
};

template <typename T>
class Grid2D
{
public:
    std::vector<std::unique_ptr<Cell<T>>> values;

    Grid2D(size_t w, size_t h) : m_width(w), m_height(h), values(w * h) {}
    // Grid2D(size_t w, size_t h, float depth, T val) : Grid2D(w, h)
    // {
    //     fill(depth, val);
    // }

    // void fill(float depth, T val)
    // {
    //     for (size_t y = 0; y < m_height; ++y)
    //         for (size_t x = 0; x < m_height; ++x)
    //             addDepth(x, y, depth, val);
    // }
    void addDepth(size_t x, size_t y, float depth, T val)
    {
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
    // const size_t depth(size_t x, size_t y) const
    // {
    //     float d = 0.0f;
    //     auto currCell = cell(x, y);
    //     while (currCell != nullptr)
    //     {
    //         d += currCell.depth;
    //         currCell = currCell.below;
    //     }
    //     return d;
    // };
    template <typename U>
    bool isValidPos(const U x, const U y)
    {
        return (0 <= x && x < m_width && 0 <= y && y < m_height);
    }
    Cell<T> *cell(size_t x, size_t y) { return values[y * m_width + x].get(); }

private:
    size_t m_width, m_height;
};
