#pragma once
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <vec3.hpp>

template <typename T>
class Grid2D
{

public:
    /*
    A Cell represents a vertical slice of an (x, y) co-ordinate in the Grid.
    Cells are "stacked" on one another and their base height is assumed to be fixed,
    ie, only the topmost Cell is expected to be modified.
    */
    struct Cell
    {
        std::unique_ptr<Cell> below = nullptr;
        float floor = 0.0f; // height until the bottom of this section
        float depth = 0.0f; // depth of this section
        T value;

        Cell() {}
        Cell(float f, float d, T val) : floor(f), depth(d), value(val) {}

        float totalDepth()
        {
            return floor + depth;
        }
        size_t numLayers()
        {
            size_t count{1};
            if (below != nullptr)
                count += below->numLayers();
            return count;
        }
    };
    using CellIterator = typename std::vector<Cell>::iterator;

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
        assert(it != end());
        assert(depth >= 0.0f);

        // Replace the Cell contents if it's not got any depth (default)
        if (it->depth <= 0.0f)
        {
            it->depth = depth;
            it->value = val;
        }
        else if (it->value == val)
        {
            it->depth += depth;
        }
        else
        {
            // Copy the current cell into a new child cell, update the current cell
            auto oldCell = std::make_unique<Cell>(std::move(*it));
            *it = Cell(oldCell->totalDepth(), depth, val);
            it->below = std::move(oldCell);
        }
    }
    void removeDepth(CellIterator it, float depth)
    {
        assert(it != end());
        assert(depth >= 0.0f);

        if (depth > it->depth)
        {
            if (it->below != nullptr)
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
                // If there's more depth being removed than available, simply zero out
                // the last Cell.
                it->depth = 0.0f;
            }
        }
        else
        {
            it->depth -= depth;
        }
    }
    vec3f normal(CellIterator it)
    {
        assert(it != end());

        int index = std::distance(begin(), it);
        int x = index % m_width;
        int y = index / m_width;

        // Central differential
        // When calculating out of bounds values, assume the edge values are extended
        float left = m_values[y * m_width + (x > 0 ? x - 1 : x)].totalDepth();
        float right = m_values[y * m_width + (x < m_width - 1 ? x + 1 : x)].totalDepth();
        float top = m_values[(y > 0 ? y - 1 : y) * m_width + x].totalDepth();
        float bottom = m_values[(y < m_height - 1 ? y + 1 : y) * m_width + x].totalDepth();

        //  https://stackoverflow.com/questions/49640250/calculate-normals-from-heightmap
        // Algorithm says (2(R-L), 2(B-T), -4) but that looks incorrect...
        // I expect positive X normals on right facing slopes and positive Y normals
        // on "north" facing slopes, as well as a near constant blue for the up vector.
        // Manually edited the sign of each value to get what I expect
        // TODO: Validate the math
        return normalise(vec3f(-2.0f * (right - left), 2.0f * (bottom - top), 4.0f));
    }

    CellIterator begin()
    {
        return m_values.begin();
    }
    CellIterator iterator(size_t x, size_t y)
    {
        if (x >= m_width || y >= m_height)
            return end();
        return m_values.begin() + y * m_width + x;
    }
    CellIterator end()
    {
        return m_values.end();
    }

private:
    size_t m_width, m_height;
    std::vector<Cell> m_values;
};
