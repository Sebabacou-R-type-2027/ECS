export module ecs:components;
import :abstractions;

import std;

export namespace ecs::components {
    using position = abstractions::vector<float>;

    using namespace std::chrono_literals;

    /**
        * @brief Component that defines the velocity

        * This component is used to define the velocity of the entity. It contains the x and y velocity of the entity.
     */
    class clock {
        std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration delta = 0s;

        public:
            virtual constexpr ~clock() noexcept = 0;

            inline void cache() noexcept { delta = std::chrono::steady_clock::now() - last; }
            inline void reset() noexcept { last = std::chrono::steady_clock::now(); }

            static inline void cache_system(clock &clock) noexcept { clock.cache(); }
            static inline void reset_system(clock &clock) noexcept { clock.reset(); }

            const std::chrono::steady_clock::duration &elapsed = delta;
    };

    constexpr clock::~clock() noexcept = default;
}
