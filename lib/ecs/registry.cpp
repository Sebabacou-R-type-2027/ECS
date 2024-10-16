module ecs;

import std;

namespace ecs {
    void registry::run_systems()
    {
        std::ranges::for_each(this->get_entities(), [this](auto e) constexpr noexcept {
            std::ranges::for_each(_systems, [this, e](const system &system) constexpr noexcept {
                system.update(*this, e);
            });
        });
    }
}
