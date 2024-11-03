#if __cpp_lib_modules < 202207L
module;

#include <chrono>
#include <memory>
#include <map>
#endif
export module ecs:components.gui;
import :core;
import :abstractions.gui;
import :components;

#if __cpp_lib_modules >= 202207L
import std;
#endif
import utils;

using namespace std::string_literals;
using namespace std::chrono_literals;

/**
    * @brief Namespace containing the GUI components

    * This namespace contains the GUI components of the ECS.
 */
export namespace ecs::components::gui {

    /**
        * @brief Component that defines the display

        * This component is used to define the display of the game. It contains the window and the element factory.
     */
    struct display {
        std::unique_ptr<abstractions::gui::window> window;
        std::unique_ptr<abstractions::gui::element_factory> factory;
    };

    /**
        * @brief Component that defines the asset

        * This component is used to define the asset of the game. It contains the type of the asset.
     */
    class asset_manager {
        std::unique_ptr<abstractions::gui::asset_loader> _loader;
        std::unordered_string_map<std::unique_ptr<abstractions::gui::asset>> _assets;

        public:
            inline asset_manager(std::unique_ptr<abstractions::gui::asset_loader> loader) noexcept
                : _loader(std::move(loader)), _assets()
            {}

            /**
                * @brief Load an asset

                * This function is used to load an asset.

                * @param name The name of the asset
                * @param path The path of the asset
                * @param type The type of the asset
                * @param replace The replace status
                * @return true if the asset was loaded, false otherwise
             */
            inline bool load(std::string_view name, std::string_view path, std::string_view type, bool replace = false) noexcept
            {
                if (replace && _assets.contains(name))
                    _assets.erase(_assets.find(name));
                try {
                    return _assets.try_emplace(std::string(name), _loader->load(path, type)).second;
                } catch (...) {
                    return false;
                }
            }

            inline const abstractions::gui::asset &get(std::string_view name) const noexcept { return *_assets.find(name)->second; }

            /**
                * @brief Unload an asset

                * This function is used to unload an asset.

                * @param name The name of the asset
             */
            inline void unload(std::string_view name) noexcept { _assets.erase(_assets.find(name)); }

            /**
                * @brief Unload an asset

                * This function is used to unload an asset.

                * @param asset The asset
             */
            inline void unload(const abstractions::gui::asset &asset) noexcept
            {
                auto it = std::find_if(_assets.begin(), _assets.end(), [&asset](const auto &pair){
                    return pair.second.get() == std::addressof(asset);
                });
                if (it != _assets.end())
                    _assets.erase(it);
            }
            inline void clear() noexcept { _assets.clear(); }
    };

    struct animation_clock : public clock {};

    /**
        * @brief Component that defines the drawable

        * This component is used to define the drawable of the game. It contains the asset manager and the elements.
     */
    struct drawable {
        using elements_container = std::multimap<std::size_t, std::unique_ptr<abstractions::gui::drawable_element>>;
        entity asset_manager;
        elements_container elements;
    };
}
