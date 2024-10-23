#if __cpp_lib_modules < 202207L
module;

#include <string>
#include <unordered_map>
#endif
export module utils:containers;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace std {
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
}
