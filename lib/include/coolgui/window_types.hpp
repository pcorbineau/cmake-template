#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "coolgui/strong_type.hpp"

namespace coolgui {

// Primitive type aliases
using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;
using i8    = std::int8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;
using usize = std::size_t;
using isize = std::ptrdiff_t;
using f32   = float;
using f64   = double;

// Window domain strong types
using Width  = StrongType<u32,              struct WidthTag>;
using Height = StrongType<u32,              struct HeightTag>;
using Title  = StrongType<std::string_view, struct TitleTag>;

} // namespace coolgui
