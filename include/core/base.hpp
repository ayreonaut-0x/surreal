#pragma once

#include "config.hpp"

#include "flags.hpp"

#include <cassert>
#include <concepts>

#define SURREAL_ASSERT(expr) assert(expr)
namespace Surreal
{

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef float f32;
typedef double f64;

template <typename Tp>
struct Size
{
    Tp w, h;

    constexpr auto operator<=>(const Size&) const noexcept = default;
};

typedef Size<i32> SizeI;
typedef Size<u32> SizeU;
typedef Size<f32> SizeF;
template <typename Tp>
struct Position
{
    Tp x, y;

    constexpr auto operator<=>(const Position&) const noexcept = default;
};

typedef Position<i32> PositionI;
typedef Position<u32> PositionU;
typedef Position<f32> PositionF;

template <typename PositionTp, typename SizeTp = PositionTp>
struct Rect
{
    PositionTp pos;
    SizeTp size;

    constexpr auto operator<=>(const Rect&) const noexcept = default;
};

} // namespace Surreal
