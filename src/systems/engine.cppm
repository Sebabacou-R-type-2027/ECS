export module ecs:systems.engine;
import :core;
import :abstractions.gui;
import :components;
import :components.gui;
import :components.engine;

import std;

export namespace ecs::systems::engine
{

    /**
        * @brief Move the entity

        * This function is used to move the entity with the given position and velocity.

        * @param pos The position of the entity
        * @param d The velocity of the entity
     */
    constexpr void movement(components::position &pos, const components::engine::velocity &d) noexcept
    {
        pos.x += d.x;
        pos.y += d.y;
    }

    /**
        * @brief Control the entity

        * This function is used to control the entity with the given position and controllable component.

        * @param ec The entity container
        * @param pos The position of the entity
        * @param c The controllable component
     */
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

    /**
        * @brief Update the hitbox position

        * This function is used to update the hitbox position with the given position and hitbox component.

        * @param pos The position of the entity
        * @param box The hitbox component
     */
    constexpr void update_hitbox_position(const ecs::components::position &pos, ecs::components::engine::hitbox &box) noexcept {
        box.area.x = pos.x;
        box.area.y = pos.y;
    }

    /**
        * @brief Handle the collision

        * This function is used to handle the collision between entities.

        * @param e The entity
        * @param ec The entity container
        * @param box The hitbox component
     */
    constexpr void collision(entity e, entity_container &ec, components::engine::hitbox &box) noexcept
    {
        bool collision_detected = false;

        std::ranges::for_each(ec.get_entities(), [&](entity other) {
            if (e == other)
                return;

            auto other_box = ec.get_entity_component<components::engine::hitbox>(other);

            if (other_box.has_value()) {
                if (box.area.intersects(other_box->get().area)) {
                    other_box->get().triggered_by = e;
                    box.triggered_by = other;
                    collision_detected = true;
                }
            }
        });

        if (!collision_detected) {
            box.triggered_by = std::nullopt;
        }
    }
}
