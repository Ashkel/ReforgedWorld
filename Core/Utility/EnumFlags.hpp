#pragma once
#include <type_traits>

/**
 * @brief Marker trait to enable bitwise operators for a specific enum.
 * Specialize or use the macro below.
 */
template<typename EnumType>
struct EnableBitMaskOperators
{
    static constexpr bool enable = false;
};

/**
 * @brief Macro to enable bitwise operators for your enum.
 * Example: ENABLE_BITMASK(MyEnum);
 */
#define ENABLE_BITMASK(EnumType) \
    template<> struct EnableBitMaskOperators<EnumType> { static constexpr bool enable = true; };

/**
 * @brief Returns the underlying value of an enum.
 */
template<typename EnumType>
constexpr auto to_underlying(EnumType e) noexcept
{
    return static_cast<std::underlying_type_t<EnumType>>(e);
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType>
operator|(EnumType lhs, EnumType rhs)
{
    return static_cast<EnumType>(to_underlying(lhs) | to_underlying(rhs));
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType>
operator&(EnumType lhs, EnumType rhs)
{
    return static_cast<EnumType>(to_underlying(lhs) & to_underlying(rhs));
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType>
operator^(EnumType lhs, EnumType rhs)
{
    return static_cast<EnumType>(to_underlying(lhs) ^ to_underlying(rhs));
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType>
operator~(EnumType lhs)
{
    return static_cast<EnumType>(~to_underlying(lhs));
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType&>
operator|=(EnumType& lhs, EnumType rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType&>
operator&=(EnumType& lhs, EnumType rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, EnumType&>
operator^=(EnumType& lhs, EnumType rhs)
{
    lhs = lhs ^ rhs;
    return lhs;
}

/**
 * @brief Checks if all bits in rhs are set in lhs.
 */
template<typename EnumType>
constexpr std::enable_if_t<EnableBitMaskOperators<EnumType>::enable, bool>
HasFlag(EnumType lhs, EnumType rhs)
{
    return (to_underlying(lhs) & to_underlying(rhs)) == to_underlying(rhs);
}
