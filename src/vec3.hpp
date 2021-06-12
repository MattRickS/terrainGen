#pragma once
#include <cmath>
#include <iostream>

template <typename T>
class vec3
{
public:
    union
    {
        T data[3];
        struct
        {
            T x;
            T y;
            T z;
        };
        struct
        {
            T r;
            T g;
            T b;
        };
    };

    vec3() : data{0, 0, 0} {}
    vec3(T x) : data{x, x, x} {}
    vec3(T x, T y, T z) : data{x, y, z} {}

    vec3 operator-() const { return vec3(-x, -y, -z); }
    T operator[](int i) const { return data[i]; }
    T &operator[](int i) { return data[i]; }

    vec3 &operator+=(const vec3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    vec3 &operator*=(const T t)
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    vec3 &operator/=(const T t)
    {
        return *this *= 1 / t;
    }
};

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const vec3<T> &v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

template <typename T>
inline vec3<T> operator+(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template <typename T>
inline vec3<T> operator+(const vec3<T> &lhs, const float &rhs)
{
    return vec3<T>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

template <typename T>
inline vec3<T> operator-(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template <typename T>
inline vec3<T> operator*(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

template <typename T>
inline vec3<T> operator*(const vec3<T> &lhs, const T rhs)
{
    return vec3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

template <typename T>
inline vec3<T> operator*(const T lhs, const vec3<T> &rhs)
{
    return rhs * lhs;
}

template <typename T>
inline vec3<T> operator/(const vec3<T> &lhs, const T rhs)
{
    return lhs * (1 / rhs);
}

template <typename T>
inline vec3<T> cross(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return vec3<T>(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

template <typename T>
inline T dot(const vec3<T> &lhs, const vec3<T> &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template <typename T>
inline T dot(const vec3<T> &v)
{
    return dot(v, v);
}

template <typename T>
inline T length(const vec3<T> &v)
{
    return std::sqrt(dot(v));
}

template <typename T>
inline vec3<T> normalise(const vec3<T> &v)
{
    return v / length(v);
}

using vec3f = vec3<float>;
using vec3i = vec3<int>;
