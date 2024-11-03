export module ecs:components.engine;
import :core;
import :abstractions;
import :components;

import std;

export namespace ecs::components::engine {

    /**
        * @brief Component that defines the velocity
        * 
        * This component is used to define the velocity of the entity. It contains the x and y velocity of the entity.
     */
    struct velocity : public position {
        using position::position;
    };

    /**
        * @brief Component that defines the controllable
        * 
        * This component is used to define the controllable of the entity. It contains the display entity, enabled status, and speed of the entity.
     */
    struct controllable {
        ecs::entity display;
        bool enabled;
        float speed;
    };

    /**
        * @brief Component that defines the hitbox
        * 
        * This component is used to define the hitbox of the entity. It contains the area and the entity that triggered the hitbox.
     */
    struct hitbox {
        abstractions::rectangle<float> area;
        std::optional<ecs::entity> triggered_by;
    };
}
