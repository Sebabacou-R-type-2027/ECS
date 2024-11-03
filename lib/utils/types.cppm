export module utils:types;

import std;

export namespace std {

    /**
        * @brief Replace a type in a type list

        * This class is used to replace a type in a type list.
     */
    template<typename T, typename>
    struct replace_type { using type = T; };

    /**
        * @brief Replace a type in a type list

        * This class is used to replace a type in a type list.
     */
    template<typename T, typename>
    using replace_type_t = typename replace_type<T, void>::type;

    /**
        * @brief Replace an index in a type list

        * This class is used to replace an index in a type list.
     */
    template<typename T, std::size_t>
    struct replace_index { using type = T; };

    /**
        * @brief Replace an index in a type list

        * This class is used to replace an index in a type list.
     */
    template<typename T, std::size_t>
    using replace_index_t = typename replace_index<T, 0>::type;

    /**
        * @brief Typeset

        * This class is used to represent a typeset.
     */
    template<typename... T>
    struct typeset_t {
        template<typename U>
        static constexpr bool contains = (std::is_same_v<T, U> || ...);

        static constexpr std::size_t size = sizeof...(T);

        template<std::size_t I>
        using type = std::tuple_element_t<I, std::tuple<T...>>;
    };

    /**
        * @brief Typeset

        * This class is used to represent a typeset.
     */
    template<typename... T>
    constexpr typeset_t<T...> typeset = {};

    /**
        * @brief Make a uniform typeset

        * This function is used to make a uniform typeset.
     */
    template<typename T, std::size_t N>
    consteval auto make_uniform_typeset() noexcept
    {
        return []<std::size_t... I>(std::index_sequence<I...>) consteval noexcept {
            return typeset<std::replace_index_t<T, I>...>();
        }(std::make_index_sequence<N>{});
    }

    /**
        * @brief Make a uniform typeset

        * This function is used to make a uniform typeset.
     */
    struct abstract {
        constexpr abstract(const abstract&) noexcept = delete;
        constexpr abstract(abstract&&) noexcept = delete;

        protected:
            constexpr abstract() noexcept = default;
    };
}
