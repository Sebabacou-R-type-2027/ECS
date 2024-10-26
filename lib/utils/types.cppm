#if __cpp_lib_modules < 202207L
module;

#include <tuple>
#include <type_traits>
#endif
export module utils:types;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace std {
    template<typename T, typename>
    struct replace_type { using type = T; };

    template<typename T, typename>
    using replace_type_t = typename replace_type<T, void>::type;

    template<typename T, std::size_t>
    struct replace_index { using type = T; };

    template<typename T, std::size_t>
    using replace_index_t = typename replace_index<T, 0>::type;

    template<typename... T>
    struct typeset_t {
        template<typename U>
        static constexpr bool contains = (std::is_same_v<T, U> || ...);

        static constexpr std::size_t size = sizeof...(T);

        template<std::size_t I>
        using type = std::tuple_element_t<I, std::tuple<T...>>;
    };

    template<typename... T>
    constexpr typeset_t<T...> typeset = {};

    template<typename T, std::size_t N>
    consteval auto make_uniform_typeset() noexcept
    {
        return []<std::size_t... I>(std::index_sequence<I...>) consteval noexcept {
            return typeset<std::replace_index_t<T, I>...>();
        }(std::make_index_sequence<N>{});
    }
}
