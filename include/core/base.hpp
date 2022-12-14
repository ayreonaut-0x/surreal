#pragma once

#include "config.hpp"

#include "flags.hpp"

#include <concepts>

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

struct Size
{
    u32 w, h;
};

struct Position
{
    u32 x, y;
};

struct Rect
{
    Position pos;
    Size size;
};

} // namespace Surreal
