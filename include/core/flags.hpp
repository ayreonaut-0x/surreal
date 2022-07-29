#pragma once

#include <compare>
#include <type_traits>

namespace Surreal
{

// C++ 23 has a nifty helper to test if a type is a scoped enum (enum class/struct).
// If C++ 23 is available we typedef std::is_scoped_enum
#if 23 <= SURREAL_CPP_VERSION

template <typename Tp>
using IsScopedEnum = std::is_scoped_enum<Tp>;

#else

// Otherwise we roll our own implementation.
namespace Detail
{

template <typename Tp>
auto _test_sizeable(int) -> decltype(sizeof(Tp), std::true_type());

template <typename>
auto _test_sizeable(...) -> std::false_type;

template <typename Tp>
auto _test_nonconvertible_to_int(int) -> decltype(static_cast<std::false_type (*)(int)>(nullptr)(std::declval<Tp>()));

template <typename>
auto _test_nonconvertible_to_int(...) -> std::true_type;

template <typename Tp>
constexpr bool _is_scoped_enum_impl{
    std::conjunction<decltype(_test_sizeable<Tp>(0)), decltype(_test_nonconvertible_to_int<Tp>(0))>::value
};

} // namespace Detail

template <typename>
struct IsScopedEnum : std::false_type
{
};

template <typename Tp>
requires(std::is_enum<Tp>::value) struct IsScopedEnum<Tp> : std::bool_constant<Detail::_is_scoped_enum_impl<Tp>>
{
};

#endif

template <typename BitsTp>
requires(IsScopedEnum<BitsTp>::value) struct FlagTraits
{
};

template <typename BitsTp>
requires(IsScopedEnum<BitsTp>::value) class Flags
{
public:
    typedef FlagTraits<BitsTp>::MaskType MaskType;

    constexpr Flags() : m_mask(MaskType(0)) {}
    constexpr Flags(BitsTp bit) : m_mask(MaskType(bit)) {}
    constexpr explicit Flags(MaskType mask) : m_mask(mask) {}

    constexpr auto operator<=>(const Flags&) const noexcept = default;

    constexpr Flags& operator&=(const Flags& that) noexcept
    {
        this->m_mask &= that.m_mask;
        return *this;
    }

    constexpr Flags& operator|=(const Flags& that) noexcept
    {
        this->m_mask |= that.m_mask;
        return *this;
    }

    constexpr Flags& operator^=(const Flags& that) noexcept
    {
        this->m_mask ^= that.m_mask;
        return *this;
    }

    constexpr Flags operator&(const Flags& that) const noexcept { return Flags(this->m_mask & that.m_mask); }
    constexpr Flags operator|(const Flags& that) const noexcept { return Flags(this->m_mask | that.m_mask); }
    constexpr Flags operator^(const Flags& that) const noexcept { return Flags(this->m_mask ^ that.m_mask); }
    constexpr Flags operator~() const noexcept { return Flags(this->m_mask ^ FlagTraits<BitsTp>::AllFlags); }

    constexpr Flags& operator&=(BitsTp bit) noexcept
    {
        this->m_mask &= MaskType(bit);
        return *this;
    }

    constexpr Flags& operator|=(BitsTp bit) noexcept
    {
        this->m_mask |= MaskType(bit);
        return *this;
    }

    constexpr Flags& operator^=(BitsTp bit) noexcept
    {
        this->m_mask ^= MaskType(bit);
        return *this;
    }

    constexpr Flags operator&(BitsTp bit) const noexcept { return Flags(bit) &= *this; }
    constexpr Flags operator|(BitsTp bit) const noexcept { return Flags(bit) |= *this; }
    constexpr Flags operator^(BitsTp bit) const noexcept { return Flags(bit) ^= *this; }

    constexpr bool operator!() const noexcept { return !m_mask; }
    constexpr explicit operator bool() const noexcept { return !!m_mask; }
    constexpr operator MaskType() const noexcept { return m_mask; }

private:
    MaskType m_mask;
};

template <typename BitsTp>
requires(IsScopedEnum<BitsTp>::value) constexpr Flags<BitsTp> operator&(BitsTp bit0, BitsTp bit1) noexcept
{
    return Flags<BitsTp>(bit0) &= bit1;
}

template <typename BitsTp>
requires(IsScopedEnum<BitsTp>::value) constexpr Flags<BitsTp> operator|(BitsTp bit0, BitsTp bit1) noexcept
{
    return Flags<BitsTp>(bit0) |= bit1;
}

template <typename BitsTp>
requires(IsScopedEnum<BitsTp>::value) constexpr Flags<BitsTp> operator^(BitsTp bit0, BitsTp bit1) noexcept
{
    return Flags<BitsTp>(bit0) ^= bit1;
}

} // namespace Surreal
