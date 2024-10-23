export module ecs:systems.engine;
import :components;
import :components.engine;

export namespace ecs::systems::engine {
    constexpr void movement(components::position &pos, const components::engine::velocity &d) noexcept
    {
        pos.x += d.x;
        pos.y += d.y;
    }
}
