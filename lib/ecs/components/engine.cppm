export module ecs:components.engine;
import :components;

export namespace ecs::components::engine {
    struct velocity : public position {
        using position::position;
    };
}
