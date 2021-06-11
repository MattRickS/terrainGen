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
public:
    std::vector<std::unique_ptr<Cell<T>>> values;

    Grid2D(size_t w, size_t h) : m_width(w), m_height(h), values(w * h) {}

    void fill(float depth, T val)
    {
        for (size_t y = 0; y < m_height; ++y)
            for (size_t x = 0; x < m_width; ++x)
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
    void removeDepth(size_t x, size_t y, float depth)
    {
        auto *topCell = cell(x, y);
        if (topCell == nullptr)
        {
            return;
        }
        else if (depth > topCell->depth)
        {
            // Remove cell and recurse with remaining depth
            float remainingDepth = depth - topCell->depth;
            values[y * m_width + x] = topCell->below;
            removeDepth(x, y, remainingDepth);
        }
        else
        {
            topCell->depth -= depth;
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

    vec3f normal(size_t x, size_t y)
    {
        // Central differential
        // When calculating out of bounds values, assume the edge values are extended
        float left = values[y * m_width + (x > 0 ? x - 1 : x)]->totalDepth();
        float right = values[y * m_width + (x < m_width - 1 ? x + 1 : x)]->totalDepth();
        float top = values[(y > 0 ? y - 1 : y) * m_width + x]->totalDepth();
        float bottom = values[(y < m_height - 1 ? y + 1 : y) * m_width + x]->totalDepth();
        return normalise(vec3f(2.0f * (right - left), 2.0f * (bottom - top), -4.0f));
    }

    template <typename U>
    bool isValidPos(const U x, const U y)
    {
        return (0 <= x && x < m_width && 0 <= y && y < m_height);
    }
    Cell<T> *cell(size_t x, size_t y) { return values[y * m_width + x].get(); }

    // std::vector<std::unique_ptr<Cell<T>>>::iterator begin()
    // {
    //     return values.begin();
    // }
    // std::vector<std::unique_ptr<Cell<T>>>::iterator cellIndex(size_t x, size_t y)
    // {
    //     return values.begin() + y * m_width + x;
    // }
    // std::vector<std::unique_ptr<Cell<T>>>::iterator end()
    // {
    //     return values.end();
    // }

private:
    size_t m_width, m_height;
};
