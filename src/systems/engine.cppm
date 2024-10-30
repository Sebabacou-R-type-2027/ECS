module;

#include <SFML/Window/Keyboard.hpp>
export module ecs:systems.engine;
import :core;
import :components;
import :components.gui;
import :components.engine;
import :abstractions;
import :core;

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

    constexpr void control(components::position &pos, const components::engine::controllable &c) noexcept
    {
        if (!c.enabled)
            return;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            pos.x -= c.speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            pos.x += c.speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            pos.y -= c.speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            pos.y += c.speed;
    }

    constexpr void collision(entity e, entity_container &ec, const components::engine::hitbox &box) noexcept
    {
        std::ranges::for_each(ec.get_entities(), [&](entity other) {
            if (e == other)
                return;
            auto other_box = ec.get_entity_component<components::engine::hitbox>(other);
            if (other_box.has_value() && box.box.intersects(other_box->get().box)) {
                std::cout << "Collision detected" << std::endl;
                other_box->get().is_trigger = true;
                box.is_trigger = true;
            }
        });
    }

}
