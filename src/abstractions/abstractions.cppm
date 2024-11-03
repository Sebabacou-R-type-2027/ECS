export module ecs:abstractions;

import std;

export namespace ecs::abstractions {

    /**
        * @brief Vector abstraction

        * This struct is used to abstract a vector.
     */
    template<typename T> requires std::integral<T> || std::floating_point<T>
    struct vector {
        constexpr vector() noexcept = default;
        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr vector(U value) noexcept : x(static_cast<T>(value)), y(static_cast<T>(value)) {}
        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr vector(U x, U y) noexcept : x(static_cast<T>(x)), y(static_cast<T>(y)) {}
        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr vector(vector<U> other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

        T x{}, y{};
    };

    /**
        * @brief Rectangle abstraction

        * This struct is used to abstract a rectangle.
     */
    template<typename T> requires std::integral<T> || std::floating_point<T>
    struct rectangle {
        constexpr rectangle() noexcept = default;
        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr rectangle(vector<U> position, vector<U> size) noexcept
            : x(static_cast<T>(position.x)), y(static_cast<T>(position.y)),
            width(static_cast<T>(size.x)), height(static_cast<T>(size.y))
        {}
        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr rectangle(U x, U y, U width, U height) noexcept
            : x(static_cast<T>(x)), y(static_cast<T>(y)),
            width(static_cast<T>(width)), height(static_cast<T>(height))
        {}

        T x{}, y{}, width{}, height{};

        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr bool contains(vector<U> point) const noexcept
        {
            const T min_x = std::min(x, x + width),
                max_x = std::max(x, x + width),
                min_y = std::min(y, y + height),
                max_y = std::max(y, y + height);

            return (static_cast<T>(point.x) >= min_x) && (static_cast<T>(point.x) < max_x)
                && (static_cast<T>(point.y) >= min_y) && (static_cast<T>(point.y) < max_y);
        }

        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr bool intersects(const rectangle<U> &other) const noexcept
        {
            return find_intersection(other).has_value();
        }

        template<typename U> requires std::integral<U> || std::floating_point<U>
        constexpr std::optional<rectangle<T>> find_intersection(const rectangle<U> &other) const noexcept
        {
            const T min_x = std::min(x, x + width),
                max_x = std::max(x, x + width),
                min_y = std::min(y, y + height),
                max_y = std::max(y, y + height),
                other_min_x = static_cast<T>(std::min(other.x, other.x + other.width)),
                other_max_x = static_cast<T>(std::max(other.x, other.x + other.width)),
                other_min_y = static_cast<T>(std::min(other.y, other.y + other.height)),
                other_max_y = static_cast<T>(std::max(other.y, other.y + other.height)),
                left = std::max(min_x, other_min_x),
                top = std::max(min_y, other_min_y),
                right = std::min(max_x, other_max_x),
                bottom = std::min(max_y, other_max_y);

            if ((left < right) && (top < bottom))
                return rectangle<T>(left, top, right - left, bottom - top);
            return std::nullopt;
        }
    };
}
