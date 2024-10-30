export module ecs:systems.engine;
import :core;
import :abstractions.gui;
import :components;
import :components.engine;
import :components.gui;

export namespace ecs::systems::engine {
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
}
