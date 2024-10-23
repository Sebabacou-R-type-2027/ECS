module;

#if __cpp_lib_modules < 202207L
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

    struct display_element {
        std::shared_ptr<sf::Drawable> element;
        std::optional<std::string> asset_key;
    };

    struct drawable {
        using elements_container = std::unordered_multimap<std::size_t, display_element>;
        entity asset_manager;
        elements_container elements;
    };
}
