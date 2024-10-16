export module ecs;

import std;

export namespace ecs {
    struct entity {
        constexpr operator std::size_t() const noexcept { return _id; }

        private:
            constexpr entity(std::size_t id) noexcept : _id(id) {}

            std::size_t _id;
    };
}
