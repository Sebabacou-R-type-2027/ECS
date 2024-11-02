export module ecs:systems.engine;
import :core;
import :abstractions.gui;
import :components;
import :components.gui;
import :components.engine;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace ecs::systems::engine
{
    constexpr void movement(components::position &pos, const components::engine::velocity &d) noexcept
    {
        pos.x += d.x;
        pos.y += d.y;
    }

    constexpr void control(entity_container &ec, components::position &pos, const components::engine::controllable &c) noexcept
    {
        if (!c.enabled)
            return;

        auto display = ec.get_entity(c.display).and_then([&ec](auto e){
            return ec.get_entity_component<components::gui::display>(e);
        });
        if (!display)
            return;
        if (display->get().window->is_input_active(abstractions::gui::inputs::left))
            pos.x -= c.speed;
        if (display->get().window->is_input_active(abstractions::gui::inputs::right))
            pos.x += c.speed;
        if (display->get().window->is_input_active(abstractions::gui::inputs::up))
            pos.y -= c.speed;
        if (display->get().window->is_input_active(abstractions::gui::inputs::down))
            pos.y += c.speed;
    }

    constexpr void update_hitbox_position(const ecs::components::position &pos, ecs::components::engine::hitbox &box) noexcept {
        box.area.x = pos.x;
        box.area.y = pos.y;
    }

    constexpr void collision(entity e, entity_container &ec, components::engine::hitbox &box) noexcept
    {
        std::ranges::for_each(ec.get_entities(), [&](entity other) {
            if (e == other)
                return;
            auto other_box = ec.get_entity_component<components::engine::hitbox>(other);
            if (other_box.has_value() && box.area.intersects(other_box->get().area) && !box.triggered && !other_box->get().triggered) {
                other_box->get().triggered = true;
                box.triggered = true;
            }
        });
    }
}
