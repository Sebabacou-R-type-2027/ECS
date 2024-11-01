export module ecs:components.engine;
import :core;
import :components;
import :abstractions;

export namespace ecs::components::engine {
    struct velocity : public position {
        using position::position;
    };

    struct controllable {
        ecs::entity display;
        bool enabled;
        float speed;
    };

    struct hitbox {
        abstractions::rectangle<float> box;
        bool is_trigger = false;
    };
}
