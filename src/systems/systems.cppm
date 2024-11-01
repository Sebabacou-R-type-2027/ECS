#if __cpp_lib_modules < 202207L
module;

#include <chrono>
#include <functional>
#include <iostream>
#endif
export module ecs:systems;
import :core;
import :components;

#if __cpp_lib_modules >= 202207L
import std;
#endif

using namespace std::chrono_literals;

export namespace ecs::systems {
    template<typename... Components>
    class logger {
        public:
            using duration = std::chrono::steady_clock::duration;
            duration period;
            std::ostream &os;

            constexpr logger(registry &registry, duration period = 1s, std::ostream &os = std::cout) noexcept
                : period(period), os(os), _registry(registry),
                _next(std::chrono::steady_clock::now()),
                _system(std::bind_front(&logger<Components...>::run, this)),
                _system_handle(_registry.register_system<Components...>(_system))
            {}

            constexpr ~logger() noexcept
            {
                _registry.remove_system(_system_handle);
            }

        protected:
            virtual void log(entity e, Components &...) const noexcept = 0;

        private:
            registry &_registry;
            std::chrono::steady_clock::time_point _next;
            std::function<void(entity, Components &...)> _system;
            const system &_system_handle;

            void run(entity e, Components &...components) noexcept
            {
                const auto now = std::chrono::steady_clock::now();
                if (_next >= now)
                    return;
                _next += period;
                log(e, components...);
            }
    };

    struct position_logger : public logger<const components::position> {
        using logger<const components::position>::logger;

        protected:
            void log(entity e, const components::position &pos) const noexcept final
            {
                os << "Entity #" << static_cast<std::size_t>(e) << ": Position: (" << pos.x << ", " << pos.y << ")" << std::endl;
            }
    };
}
