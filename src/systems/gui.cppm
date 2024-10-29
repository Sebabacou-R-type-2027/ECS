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
    void reposition(components::gui::drawable &drawable, const components::position &position) noexcept
    {
        for (auto &pair : drawable.elements) {
            if (pair.second->reposition) {
                (*pair.second->reposition)(*pair.second, position);
                continue;
            }
            if (auto *sprite = dynamic_cast<sf::Sprite *>(pair.second->element.get()))
                sprite->setPosition(position.x, position.y);
            else if (auto *text = dynamic_cast<sf::Text *>(pair.second->element.get()))
                text->setPosition(position.x, position.y);
            else if (auto *shape = dynamic_cast<sf::Shape *>(pair.second->element.get()))
                shape->setPosition(position.x, position.y);
        }
    }

    void draw(entity_container &ec, const components::gui::drawable &drawable) noexcept
    {
        for (const auto &pair : drawable.elements) {
            if (!pair.second->visible)
                continue;
            auto entity = ec.get_entity(pair.first);
            auto window = entity.and_then([&ec](auto e){
                return ec.get_entity_component<components::gui::window>(e);
            });
            auto clock = entity.and_then([&ec](auto e){
                return ec.get_entity_component<const components::gui::animation_clock>(e);
            });
            auto *animation = dynamic_cast<components::gui::animation *>(pair.second.get());
            if (animation && clock)
                animation->update(clock->get().delta);
            if (window)
                window->get().window->draw(*pair.second->element);
        }
    }

    inline void update_clock(components::gui::animation_clock &clock) noexcept { clock.update(); }

    inline void reset_clock(components::gui::animation_clock &clock) noexcept { clock.start = std::chrono::steady_clock::now(); }

    inline void clear(components::gui::window &component) noexcept { component.window->clear(); }

    inline void display(components::gui::window &component) noexcept { component.window->display(); }
}
