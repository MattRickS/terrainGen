#pragma once
#include <vector>
#include <vec3.hpp>

template <typename T>
struct Cell
{
    Cell *upper = nullptr;
    Cell *lower = nullptr;
    T value;

    Cell() {}
    Cell(T val) : value(val) {}
};

template <typename T>
class Grid2D
{
public:
    std::vector<Cell<T>> values;

    Grid2D(size_t w, size_t h) : m_width(w), m_height(h), values(w * h) {}
    Grid2D(size_t w, size_t h, T val) : Grid2D(w, h)
    {
        fill(val);
    }

    void fill(T val)
    {
        for (size_t y = 0; y < m_height; ++y)
            for (size_t x = 0; x < m_height; ++x)
                values[y * m_width + x].value = val;
    }
    void addLayer(size_t x, size_t y, T val)
    {
        auto origCell = cell(x, y);
        Cell<T> newCell(val);
        newCell.lower = &origCell;
        origCell.upper = &newCell;
        values[y * m_width + x] = newCell;
    }
    const size_t width() const { return m_height; };
    const size_t height() const { return m_width; };
    const size_t size() const { return m_width * m_height; };
    template <typename U>
    bool isValidPos(const U x, const U y)
    {
        return (0 <= x && x < m_width && 0 <= y && y < m_height);
    }
    Cell<T> &cell(size_t x, size_t y) { return values[y * m_width + x]; }

private:
    size_t m_width, m_height;
};
