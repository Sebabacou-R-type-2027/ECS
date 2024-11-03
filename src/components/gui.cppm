export module ecs:components.gui;
import :core;
import :abstractions.gui;
import :components;

import std;
import utils;

using namespace std::string_literals;
using namespace std::chrono_literals;

export namespace ecs::components::gui {
    struct display {
        std::unique_ptr<abstractions::gui::window> window;
        std::unique_ptr<abstractions::gui::element_factory> factory;
    };

    class asset_manager {
        std::unique_ptr<abstractions::gui::asset_loader> _loader;
        std::unordered_string_map<std::unique_ptr<abstractions::gui::asset>> _assets;

        public:
            inline asset_manager(std::unique_ptr<abstractions::gui::asset_loader> loader) noexcept
                : _loader(std::move(loader)), _assets()
            {}

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

            inline void unload(std::string_view name) noexcept { _assets.erase(_assets.find(name)); }
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

    struct drawable {
        using elements_container = std::multimap<std::size_t, std::unique_ptr<abstractions::gui::drawable_element>>;
        entity asset_manager;
        elements_container elements;
    };
}
