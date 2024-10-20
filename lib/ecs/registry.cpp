#if __cpp_lib_modules < 202207L
module;

#include <algorithm>
#endif
module ecs;

#if __cpp_lib_modules >= 202207L
import std;
#endif

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
