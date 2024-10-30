#if __cpp_lib_modules < 202207L
module;

#include <chrono>
#include <memory>
#include <unordered_map>
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

export namespace ecs::components::gui {
    struct display {
        std::unique_ptr<abstractions::gui::window> window;
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
        using elements_container = std::unordered_multimap<std::size_t, std::unique_ptr<abstractions::gui::drawable_element>>;
        entity asset_manager;
        elements_container elements;
    };

    class animation : public abstractions::gui::sprite {
        public:
            const std::size_t frame_lines, frame_columns;

        private:
            const std::size_t _image_width, _image_height;

        public:
            std::chrono::steady_clock::duration frame_length;

        private:
            std::size_t _frame_line_index{}, _frame_column_index{};
            std::chrono::steady_clock::duration _frame_duration{};

        public:
            animation(const abstractions::gui::texture &texture, std::size_t lines, std::size_t columns,
                std::chrono::steady_clock::duration length = 10ms) noexcept
                : frame_lines(lines), frame_columns(columns),
                _image_width(texture.get_size().x / columns), _image_height(texture.get_size().y / lines),
                frame_length(length)
            {}

            void update(std::chrono::steady_clock::duration delta) noexcept
            {
                _frame_duration += delta;
                if (_frame_duration < frame_length)
                    return;
                _frame_duration %= frame_length;

                this->set_render_area({_image_width * _frame_column_index,
                    _image_height * _frame_line_index,
                    _image_width, _image_height});

                if (++_frame_column_index < frame_columns)
                    return;

                _frame_column_index = 0;
                if (++_frame_line_index == frame_lines)
                    _frame_line_index = 0;
            }
    };
}
