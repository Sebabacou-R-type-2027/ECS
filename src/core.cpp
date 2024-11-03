module ecs;
import :core;
import :components;
import :components.engine;
import :components.gui;
import :systems;
import :systems.engine;
import :systems.gui;

namespace ecs {

    /**
        * @brief Run all systems on all entities

        * This function is used to run all systems on all entities.
     */
    void registry::register_gui_systems() noexcept
    {
        this->register_system<components::gui::display>(systems::gui::clear);
        this->register_system<components::gui::display>(systems::gui::display);
        this->register_system<components::gui::animation_clock>(components::clock::reset_system);
        this->register_system<const components::gui::drawable>(systems::gui::draw);
        this->register_system<components::gui::animation_clock>(components::clock::cache_system);
        this->register_system<components::gui::drawable, const components::position>(systems::gui::reposition);
    }

    /**
        * @brief Register the GUI systems

        * This function is used to register the GUI systems.
     */
    void registry::register_engine_systems() noexcept
    {
        this->register_system<components::position, const components::engine::velocity>(systems::engine::movement);
        this->register_system<const components::position, components::engine::hitbox>(systems::engine::update_hitbox_position);
        this->register_system<components::position, const components::engine::controllable>(systems::engine::control);
        this->register_system<components::engine::hitbox>(systems::engine::collision);
    }
}
