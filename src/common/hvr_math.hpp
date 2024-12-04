// SPDX-License-Identifier: GPL-2.0-only

#ifndef HVR_MATH_HPP
#define HVR_MATH_HPP

#include <cmath>
#include <cstdint>

namespace hvr::math {
template <class T>
struct vec3 {
    T x = 0;
    T y = 0;
    T z = 0;

    inline constexpr vec3() = default;

    inline constexpr vec3(T _x, T _y, T _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }

    inline constexpr vec3(const vec3& v) = default;

    inline constexpr vec3& operator=(const vec3& v) = default;

    inline auto mag() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    inline auto mag2() const
    {
        return (x * x + y * y + z * z);
    }

    inline constexpr vec3 floor() const
    {
        return vec3(std::floor(x), std::floor(y), std::floor(z));
    }

    inline vec3 norm() const
    {
        auto r = 1 / mag();
        return vec3(x * r, y * r, z * r);
    }

    template <class F>
    inline constexpr operator vec3<F>() const
    {
        return { static_cast<F>(this->x), static_cast<F>(this->y), static_cast<F>(this->z) };
    }
};

template <class TL, class TR>
inline constexpr auto operator*(const TL& lhs, const vec3<TR>& rhs)
{
    return vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

template <class TL, class TR>
inline constexpr auto operator*(const vec3<TL>& lhs, const TR& rhs)
{
    return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

template <class TL, class TR>
inline constexpr auto operator*(const vec3<TL>& lhs, const vec3<TR>& rhs)
{
    return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

template <class TL, class TR>
inline constexpr auto operator*=(vec3<TL>& lhs, const TR& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

template <class TL, class TR>
inline constexpr auto operator+(const TL& lhs, const vec3<TR>& rhs)
{
    return vec3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
}

template <class TL, class TR>
inline constexpr auto operator+(const vec3<TL>& lhs, const TR& rhs)
{
    return vec3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
}

template <class TL, class TR>
inline constexpr auto operator+(const vec3<TL>& lhs, const vec3<TR>& rhs)
{
    return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template <class TL, class TR>
inline constexpr auto operator+=(vec3<TL>& lhs, const TR& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

template <class TL, class TR>
inline constexpr auto operator+=(vec3<TL>& lhs, const vec3<TR>& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

template <class T>
struct quat {
    T w, x, y, z;
};

using vec3f = vec3<float>;
using vec3d = vec3<double>;
using vec3i = vec3<int32_t>;

using quatf = quat<float>;
using quatd = quat<double>;
using quati = quat<int32_t>;
}

#endif // #ifndef HVR_MATH_HPP
