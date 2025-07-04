#pragma once

#include <utility>

namespace FunctionBinder
{
    // Bind a member function pointer to an object pointer, returning a lambda
    template <typename C, typename MemFn>
    constexpr auto Bind(C* obj, MemFn memFn)
    {
        return [obj, memFn](auto&&... args) -> decltype(auto)
            {
                return (obj->*memFn)(std::forward<decltype(args)>(args)...);
            };
    }

    // Overload for const object pointer
    template <typename C, typename MemFn>
    constexpr auto Bind(const C* obj, MemFn memFn)
    {
        return [obj, memFn](auto&&... args) -> decltype(auto)
            {
                return (obj->*memFn)(std::forward<decltype(args)>(args)...);
            };
    }
}
