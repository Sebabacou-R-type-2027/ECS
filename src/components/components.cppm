#if __cpp_lib_modules < 202207L
module;

#include <chrono>
#endif
export module ecs:components;
import :abstractions;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace ecs::components {
    using position = abstractions::vector<float>;

    using namespace std::chrono_literals;

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

    struct lifestate {
        constexpr lifestate() noexcept : alive{true} {}
        constexpr lifestate(bool a) noexcept : alive{a} {}
        bool alive;
    };

    constexpr clock::~clock() noexcept = default;
}
