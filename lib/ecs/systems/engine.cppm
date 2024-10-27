module;

#include <SFML/Window/Keyboard.hpp>
export module ecs:systems.engine;
import :components;
import :components.engine;

export namespace ecs::systems::engine {
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
}
