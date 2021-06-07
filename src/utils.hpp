#pragma once
template <typename T>
inline double clamp(T x, T min, T max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}
