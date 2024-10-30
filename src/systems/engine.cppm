module;

#include <SFML/Window/Keyboard.hpp>
export module ecs:systems.engine;
import :core;
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
        std::ranges::for_each(ec.get_entities(), [e, &ec, &hit](auto entity) {
            if (entity == e) return;

            auto hitbox = ec.get_entity_component<components::engine::hitbox>(entity);
            auto lifestate = ec.get_entity_component<components::lifestate>(entity);
            auto life = ec.get_entity_component<components::lifestate>(e);

            if (hitbox.has_value() && 
                hitbox->get().left < hit.left + hit.width &&
                hitbox->get().left + hitbox->get().width > hit.left &&
                hitbox->get().top < hit.top + hit.height &&
                hitbox->get().height + hitbox->get().top > hit.top) 
            {   
                ec.erase_entity(e);
                ec.erase_entity(entity);

                std::cout << "Collision between " << e << " and " << entity << std::endl;
                // Récupérer et afficher les `texture_id`
                auto drawable_e = ec.get_entity_component<ecs::components::gui::drawable>(e);
                auto drawable_entity = ec.get_entity_component<ecs::components::gui::drawable>(entity);

                if (drawable_e.has_value()) {
                    for (const auto &element_pair : drawable_e->get().elements) {
                        if (auto &display_elem = element_pair.second) {
                            if (display_elem->asset_key.has_value()) {
                                std::cout << "Entity " << e << " texture_id: " << *display_elem->asset_key << std::endl;
                            }
                        }
                    }
                }

                if (drawable_entity.has_value()) {
                    for (const auto &element_pair : drawable_entity->get().elements) {
                        if (auto &display_elem = element_pair.second) {
                            if (display_elem->asset_key.has_value()) {
                                std::cout << "Entity " << entity << " texture_id: " << *display_elem->asset_key << std::endl;
                            }
                        }
                    }
                }
            }
        });
    }

}
