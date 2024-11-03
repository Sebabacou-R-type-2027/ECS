module;

#if __cpp_lib_modules < 202207L
#include <string>
#include <unordered_map>
#endif
export module utils:containers;

import std;

export namespace std {

    /**
        * @brief Hash function for strings

        * This struct is used to define the hash function for strings.
     */
    struct string_hash
    {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        std::size_t operator()(const char *str) const        { return hash_type{}(str); }
        std::size_t operator()(std::string_view str) const   { return hash_type{}(str); }
        std::size_t operator()(const std::string &str) const { return hash_type{}(str); }
    };

    template<typename T, typename Allocator = std::allocator<std::pair<const std::string, T>>>
    using unordered_string_map = std::unordered_map<std::string, T, string_hash, std::equal_to<>, Allocator>;

    /**
        * @brief Unique initializer

        * This class is used to define a unique initializer.
     */
    template<typename T>
    class unique_intializer {
        mutable T value;

        public:
            constexpr unique_intializer(T &&v) noexcept : value(std::move(v)) {}

            template<typename... Args>
            constexpr unique_intializer(Args &&...args) : value{std::forward<Args>(args)...} {}

            constexpr operator T() const && noexcept { return std::move(value); }
            constexpr T move() const && noexcept { return std::move(value); }
    };

    /**

     */
    template<typename T>
    class unique_intializer<const T> : public unique_intializer<T> {
        using unique_intializer<T>::unique_intializer;
    };

    /**
        * @brief Container

        * This struct is used to define a container.
     */
    template<typename Container, typename T = typename Container::value_type>
    struct container {
        static constexpr Container make(std::initializer_list<unique_intializer<T>> il) noexcept
        {
            return Container(std::make_move_iterator(il.begin()), std::make_move_iterator(il.end()));
        }
    };

    /**
        * @brief Container

        * This struct is used to define a container.
     */
    template<typename Container, typename K, typename V>
    struct container<Container, std::pair<K, V>> {
        static constexpr Container make(std::initializer_list<unique_intializer<std::pair<std::remove_const_t<K>, std::remove_const_t<V>>>> il) noexcept
        {
            Container c{};
            for (auto &&entry : il)
                c.insert(std::move(entry).move());
            return c;
        }
    };
}
