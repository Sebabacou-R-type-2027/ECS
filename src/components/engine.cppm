export module ecs:components.engine;
import :core;
import :abstractions;
import :components;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace ecs::components::engine {
    struct velocity : public position {
        using position::position;
    };

    struct controllable {
        ecs::entity display;
        bool enabled;
        float speed;
        int network_id;
    };

    struct hitbox {
        abstractions::rectangle<float> area;
        std::optional<ecs::entity> triggered_by;
    };
}
