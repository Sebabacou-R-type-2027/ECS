module;

#include <SFML/Graphics.hpp>
export module ecs:systems.gui;
import :core;
import :components;
import :components.gui;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace ecs::systems::gui {
    void reposition(entity_container &ec, components::gui::drawable &drawable, const components::position &position, components::gui::animations &animations) noexcept
    {
        // components::gui::asset_manager &asset_manager = *ec.get_entity_component<components::gui::asset_manager>(drawable.asset_manager);
        for (auto &pair : drawable.elements) {
            if (auto *sprite = dynamic_cast<sf::Sprite *>(pair.second.element.get()); sprite) {
                sprite->setPosition(position.x, position.y);
                // sprite->setTexture(asset_manager.get_texture(pair.second.asset_key.value()));
            } else if (auto *text = dynamic_cast<sf::Text *>(pair.second.element.get()); text) {
                text->setPosition(position.x, position.y);
                // text->setFont(asset_manager.get_font(pair.second.asset_key.value()));
            } else if (auto *shape = dynamic_cast<sf::Shape *>(pair.second.element.get()); shape)
                shape->setPosition(position.x, position.y);
        }
        for (auto &pair : animations.elements)
            pair.second.sprite.setPosition(position.x, position.y);
    }

    void animate(entity_container &ec, components::gui::animations &animation) noexcept
    {
        for (auto &pair : animation.elements) {
            auto entity = ec.get_entity(pair.first);
            auto window = entity.and_then([&ec](auto e){
                return ec.get_entity_component<components::gui::window>(e);
            });
            auto clock = entity.and_then([&ec](auto e){
                return ec.get_entity_component<components::gui::animation_clock>(e);
            });
            if (clock) {
                pair.second.update(clock->get().delta);
            }
            if (window) {
                window->get().window.draw(pair.second.sprite);
            }
        }
    }

    void draw(entity_container &ec, const components::gui::drawable &drawable) noexcept
    {
        for (const auto &pair : drawable.elements) {
            auto component = ec.get_entity(pair.first).and_then([&ec](auto e){
                return ec.get_entity_component<components::gui::window>(e);
            });
            if (component)
                component->get().window.draw(*pair.second.element);
        }
    }

    void clear(components::gui::window &component) noexcept
    {
        component.window.clear();
    }

    void display(components::gui::window &component) noexcept
    {
        component.window.display();
    }
}
