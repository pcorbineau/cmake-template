#pragma once

#include <type_traits>
#include <utility>

namespace coolgui {

template<typename T, typename Tag>
class StrongType {
    T value_;

public:
    explicit constexpr StrongType(T val) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(val))
    {}

    template<typename Self>
    [[nodiscard]] constexpr auto get(this Self&& self) noexcept -> auto&&
    {
        return std::forward<Self>(self).value_;
    }
};

} // namespace coolgui
