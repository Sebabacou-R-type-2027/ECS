export module ecs:components.engine;
import :components;
import :core;

export namespace ecs::components::engine {
    struct velocity : public position {
        using position::position;
    };

    struct controllable {
        bool enabled;
        float speed;
    };

    struct hitbox {
        float width;
        float height;
        float top;
        float left;

        hitbox(float width, float height, float top, float left)
            : width(width), height(height), top(top), left(left)
        {}
    };
}
