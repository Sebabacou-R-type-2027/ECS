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


        void update() noexcept {
            delta = std::chrono::steady_clock::now() - start;
        }
    };
    class animation {
        private:
            int ImageSize_x;
            int ImageSize_y;
        public:
            sf::Sprite sprite;
            std::optional<std::string> assetKey;
            int imageCount_x;
            int imageCount_y;
            int currentImage_x;
            int currentImage_y;
            std::chrono::steady_clock::duration switchTime;
            std::chrono::steady_clock::duration totalTime;

        animation(components::gui::asset_manager &assets, const std::string &textureKey, int imageCountX, int imageCountY, std::chrono::steady_clock::duration switchTime)
            : assetKey(textureKey), imageCount_x(imageCountX), imageCount_y(imageCountY), currentImage_x(0), currentImage_y(0), switchTime(switchTime) {

            const sf::Texture &texture = assets.get_texture(textureKey);
            ImageSize_x = texture.getSize().x / imageCount_x;
            ImageSize_y = texture.getSize().y / imageCount_y;
            sprite.setTexture(texture);
        }

        void update(std::chrono::steady_clock::duration delta) {

            sprite.setTextureRect(sf::IntRect(currentImage_x * ImageSize_x,
                                            currentImage_y * ImageSize_y,
                                            ImageSize_x,
                                            ImageSize_y));
            totalTime += delta;

            if (totalTime >= switchTime) {
                totalTime -= switchTime;

                currentImage_x++;
                if (currentImage_x >= imageCount_x) {
                    currentImage_x = 0;
                    currentImage_y++;

                    if (currentImage_y >= imageCount_y) {
                        currentImage_y = 0;
                    }
                }

            }
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

    struct animations {
        using elements_container = std::unordered_multimap<std::size_t, animation>;
        entity asset_manager;
        elements_container elements;
    };
}
