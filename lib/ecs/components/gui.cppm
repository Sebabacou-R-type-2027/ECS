module;

#if __cpp_lib_modules < 202207L
#include <chrono>
#include <memory>
#include <unordered_map>
#endif
#include <SFML/Graphics.hpp>
export module ecs:components.gui;
import :core;
import :components;

#if __cpp_lib_modules >= 202207L
import std;
#endif
import utils;

using namespace std::string_literals;
using namespace std::chrono_literals;

export namespace ecs::components::gui {
    struct window {
        sf::RenderWindow &window;
    };

    class asset_manager {
        std::unordered_string_map<sf::Texture> _textures;
        std::unordered_string_map<sf::Font> _fonts;

        public:
            inline bool load_font(std::string_view name, std::string_view path) noexcept
            {
                return _fonts.emplace(name, sf::Font()).first->second.loadFromFile(std::string(path));
            }

            inline bool load_texture(std::string_view name, std::string_view path, const sf::IntRect &area = sf::IntRect()) noexcept
            {
                return _textures.emplace(name, sf::Texture()).first->second.loadFromFile(std::string(path), area);
            }

            inline const sf::Texture &get_texture(std::string_view name) const
            {
                auto it = _textures.find(name);
                if (it == _textures.end())
                    throw std::runtime_error(std::format("ecs: asset_manager: No such texture: {}", name));
                return it->second;
            }

            inline const sf::Font &get_font(std::string_view name) const
            {
                auto it = _fonts.find(name);
                if (it == _fonts.end())
                    throw std::runtime_error(std::format("ecs: asset_manager: No such font: {}", name));
                return it->second;
            }
    };

    struct animation_clock {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration delta;

        inline void update() noexcept
        {
            delta = std::chrono::steady_clock::now() - start;
        }
    };

    struct display_element {
        std::shared_ptr<sf::Drawable> element;
        std::optional<std::string> asset_key;
    };

    struct drawable {
        using elements_container = std::unordered_multimap<std::size_t, display_element>;
        entity asset_manager;
        elements_container elements;
    };

    class animation {
        public:
            const std::size_t frame_lines, frame_columns;
            std::optional<std::string> asset_key;

        private:
            const std::size_t _image_width, _image_height;

        public:
            sf::Sprite sprite;
            std::chrono::steady_clock::duration frame_length = 10ms;

        private:
            std::size_t _frame_line_index, _frame_column_index;
            std::chrono::steady_clock::duration _frame_duration;

        public:
            animation(const sf::Texture &texture, std::size_t lines, std::size_t columns, std::optional<std::string_view> texture_id = std::nullopt) noexcept
                : frame_lines(lines), frame_columns(columns), asset_key(texture_id),
                _image_width(texture.getSize().x / columns), _image_height(texture.getSize().y / lines),
                sprite(texture, sf::IntRect(0, 0, _image_width, _image_height))
            {}

            void update(std::chrono::steady_clock::duration delta) noexcept
            {
                _frame_duration += delta;
                if (_frame_duration < frame_length)
                    return;
                _frame_duration %= frame_length;

                sprite.setTextureRect(
                    sf::IntRect(_image_width * _frame_column_index,
                        _image_height * _frame_line_index,
                        _image_width, _image_height));

                if (++_frame_column_index < frame_columns)
                    return;

                _frame_column_index = 0;
                if (++_frame_line_index == frame_lines)
                    _frame_line_index = 0;
            }
    };

    struct animations {
        using elements_container = std::unordered_multimap<std::size_t, animation>;
        entity asset_manager;
        elements_container elements;
    };
}
