#pragma once
#include <iostream>
#include <vector>
#include <utils.hpp>
#include <glm/glm.hpp>

void startPPM(std::ostream &out, size_t width, size_t height)
{
    out << "P3" << std::endl
        << width << ' ' << height << std::endl
        << 255 << std::endl;
}

void writeColor(std::ostream &out, glm::vec3 col)
{
    out << static_cast<int>(256 * clamp(col.x, 0.0f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(col.y, 0.0f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(col.z, 0.0f, 0.999f)) << std::endl;
}

void writeColor(std::ostream &out, glm::ivec3 col)
{
    out << clamp(col.x, 0, 255) << ' '
        << clamp(col.y, 0, 255) << ' '
        << clamp(col.z, 0, 255) << std::endl;
}

void writeColor(std::ostream &out, float col)
{
    int x = static_cast<int>(256 * clamp(col, 0.0f, 0.999f));
    out << x << ' '
        << x << ' '
        << x << std::endl;
}