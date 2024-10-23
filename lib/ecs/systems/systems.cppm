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
            std::chrono::steady_clock::duration period = 1s;
            std::ostream &out = std::cout;

        protected:
            constexpr logger(registry &registry) noexcept
                : _next(std::chrono::steady_clock::now()), _registry(registry),
                _system(std::bind_front(&logger<Components...>::run, this)),
                _system_handle(_registry.register_system<Components...>(_system))
            {}

            constexpr ~logger() noexcept
            {
                _registry.remove_system(_system_handle);
            }

            virtual void log(entity e, Components &...) const noexcept = 0;

        private:
            std::chrono::steady_clock::time_point _next;
            registry &_registry;
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

    class position_logger : public logger<const components::position> {
        public:
            constexpr position_logger(registry &registry) noexcept
                : logger<const components::position>(registry)
            {}

        protected:
            void log(entity e, const components::position &pos) const noexcept final
            {
                out << "Entity #" << static_cast<std::size_t>(e) << ": Position: (" << pos.x << ", " << pos.y << ")" << std::endl;
            }
    };
}
