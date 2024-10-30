export module ecs:components;

export namespace ecs::components {
    struct position {
        constexpr position() noexcept : x{0.0f}, y{0.0f} {}
        constexpr position(float a, float b) noexcept : x{a}, y{b} {}

        float x, y;
    };

    struct lifestate {
        constexpr lifestate() noexcept : alive{true} {}
        constexpr lifestate(bool a) noexcept : alive{a} {}
        bool alive;
    };
}
