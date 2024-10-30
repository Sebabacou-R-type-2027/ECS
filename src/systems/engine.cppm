module;

#include <SFML/Window/Keyboard.hpp>
export module ecs:systems.engine;
import :core;
import :components;
import :components.engine;

#if __cpp_lib_modules >= 202207L
import std;
#endif

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

    constexpr void collision(entity e, entity_container &ec, const components::engine::hitbox &hit) noexcept
    {
        std::vector<ecs::entity> entities_to_erase;

        std::ranges::for_each(ec.get_entities(), [e, &ec, &hit, &entities_to_erase](auto entity) {
            if (entity == e) return;

            auto hitbox = ec.get_entity_component<components::engine::hitbox>(entity);
            if (hitbox.has_value() && 
                hitbox->get().left < hit.left + hit.width &&
                hitbox->get().left + hitbox->get().width > hit.left &&
                hitbox->get().top < hit.top + hit.height &&
                hitbox->get().height + hitbox->get().top > hit.top) 
            {
                if (std::find(entities_to_erase.begin(), entities_to_erase.end(), entity) == entities_to_erase.end()) {
                    entities_to_erase.push_back(entity);
                }
            }
        });

        for (auto e : entities_to_erase)
            ec.erase_entity(e);
    }
}
