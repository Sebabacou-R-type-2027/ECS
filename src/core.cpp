module ecs;
import :core;
import :components;
import :components.engine;
import :components.gui;
import :systems;
import :systems.engine;
import :systems.gui;

namespace ecs {
    void registry::register_gui_systems() noexcept
    {
        this->register_system<components::gui::display>(systems::gui::display);
        this->register_system<components::gui::animation_clock>(components::clock::reset_system);
        this->register_system<const components::gui::drawable>(systems::gui::draw);
        this->register_system<components::gui::animation_clock>(components::clock::cache_system);
        this->register_system<components::gui::drawable, const components::position>(systems::gui::reposition);
        this->register_system<ecs::components::gui::display>(ecs::systems::gui::draw_shader_background);
        this->register_system<components::gui::display>(systems::gui::clear);
    }

    void registry::register_engine_systems() noexcept
    {
        this->register_system<components::position, const components::engine::velocity>(systems::engine::movement);
        this->register_system<components::position, const components::engine::controllable>(systems::engine::control);
    }
}
