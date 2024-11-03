export module ecs:systems.gui;
import :core;
import :abstractions.gui;
import :components;
import :components.gui;

export namespace ecs::systems::gui {

    /**
        * @brief Clear the display

        * This function is used to clear the display.

        * @param display The display component
        * @param position The position of the drawable
     */
    void reposition(components::gui::drawable &drawable, const components::position &position) noexcept
    {
        for (auto &pair : drawable.elements)
            pair.second->reposition(position);
    }

    /**
        * @brief Draw the drawable

        * This function is used to draw the drawable.

        * @param ec The entity container
        * @param drawable The drawable component
     */
    void draw(entity_container &ec, const components::gui::drawable &drawable) noexcept
    {
        for (const auto &pair : drawable.elements) {
            if (!pair.second->is_visible())
                continue;
            auto entity = ec.get_entity(pair.first);
            auto display = entity.and_then([&ec](auto e){
                return ec.get_entity_component<components::gui::display>(e);
            });
            auto clock = entity.and_then([&ec](auto e){
                return ec.get_entity_component<const components::gui::animation_clock>(e);
            });
            auto *animation = dynamic_cast<abstractions::gui::animation *>(pair.second.get());
            if (animation && clock)
                animation->update(clock->get().elapsed);
            if (display)
                display->get().window->draw(*pair.second);
        }
    }

    inline void clear(components::gui::display &display) noexcept { display.window->clear(); }
    inline void display(components::gui::display &display) noexcept { display.window->display(); }
    inline void draw_shader_background(components::gui::display &display) noexcept { display.window->draw_shader_background(); }
}
