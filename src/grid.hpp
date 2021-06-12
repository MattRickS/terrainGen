#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <vec3.hpp>

template <typename T>
struct Cell
{
    // TODO: Add baseDepth for tracking the depth to the base of this Cell
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
    using CellIterator = typename std::vector<Cell<T>>::iterator;

public:
    Grid2D(size_t w, size_t h) : m_width(w), m_height(h), m_values(w * h) {}

    void fill(float depth, T val)
    {
        for (size_t y = 0; y < m_height; ++y)
            for (size_t x = 0; x < m_width; ++x)
                addDepth(x, y, depth, val);
    }
    const size_t width() const { return m_height; };
    const size_t height() const { return m_width; };
    const size_t size() const { return m_width * m_height; };
    void addDepth(CellIterator it, float depth, T val)
    {
        // assert(it != end(), "Iterator out of range");
        // assert(depth > 0.0f, "Can't add negative depth");
        if (it == end())
            throw("Out of bounds");
        else if (depth < 0.0f)
            throw("Cannot add negative depth");

        // Replace the Cell contents if it's not got any depth (default)
        if (it->depth <= 0.0f)
        {
            *it = Cell<T>(depth, val);
        }
        else
        {
            // Copy the current cell into a new child cell, update the current cell
            auto oldCell = std::make_unique<Cell<T>>(std::move(*it));
            *it = Cell<T>(depth, val);
            it->below = std::move(oldCell);
        }
    }
    void removeDepth(CellIterator it, float depth)
    {
        // TODO: Handle negative depth
        if (it == end())
        {
            throw("Out of bounds");
        }
        else if (depth > it->depth)
        {
            // Remove cell and recurse with remaining depth
            float remainingDepth = depth - it->depth;
            // move the unique pointer contents out into the vector. The vector now manages
            // the memory, and unique ptr cleans up without removing anything important.
            *it = std::move(*(it->below));
            removeDepth(it, remainingDepth);
        }
        else
        {
            it->depth -= depth;
        }
    }
    vec3f normal(size_t x, size_t y)
    {
        // Central differential
        // When calculating out of bounds values, assume the edge values are extended
        float left = m_values[y * m_width + (x > 0 ? x - 1 : x)].totalDepth();
        float right = m_values[y * m_width + (x < m_width - 1 ? x + 1 : x)].totalDepth();
        float top = m_values[(y > 0 ? y - 1 : y) * m_width + x].totalDepth();
        float bottom = m_values[(y < m_height - 1 ? y + 1 : y) * m_width + x].totalDepth();
        return normalise(vec3f(2.0f * (right - left), 2.0f * (bottom - top), -4.0f));
    }

    template <typename U>
    bool isValidPos(const U x, const U y)
    {
        return (0 <= x && x < m_width && 0 <= y && y < m_height);
    }

    CellIterator begin()
    {
        return m_values.begin();
    }
    CellIterator cellIndex(size_t x, size_t y)
    {
        return m_values.begin() + y * m_width + x;
    }
    CellIterator end()
    {
        return m_values.end();
    }
    CellIterator deepestCell() const
    {
        return std::max_element(begin(), end(), [](const auto &lhs, const auto &rhs) -> bool
                                { lhs->totalDepth() < rhs->totalDepth(); });
    }
    CellIterator mostLayeredCell() const
    {
        return std::max_element(begin(), end(), [](const auto &lhs, const auto &rhs) -> bool
                                { lhs->numLayers() < rhs->numLayers(); });
    }

private:
    size_t m_width, m_height;
    std::vector<Cell<T>> m_values;
};
