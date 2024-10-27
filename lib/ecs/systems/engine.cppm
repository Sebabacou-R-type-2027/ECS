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

    constexpr void control(components::engine::velocity &d, const components::engine::controllable &c) noexcept
    {
        if (c.enabled) {
            d.x = 0;
            d.y = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                d.x -= c.speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                d.x += c.speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                d.y -= c.speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                d.y += c.speed;
        }
    }
}
