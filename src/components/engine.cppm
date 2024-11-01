export module ecs:components.engine;
import :core;
import :components;

export namespace ecs::components::engine {
    struct velocity : public position {
        using position::position;
    };

    struct controllable {
        ecs::entity display;
        bool enabled;
        float speed;
    };
}
