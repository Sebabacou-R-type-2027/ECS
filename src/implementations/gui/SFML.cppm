module;

#if __cpp_lib_modules < 202207L
#endif
#include <SFML/Graphics.hpp>
export module ecs:implementations.gui.SFML;
import :abstractions;
import :abstractions.gui;
import :components.gui;

#if __cpp_lib_modules >= 202207L
import std;
#endif

export namespace ecs::implementations::gui::SFML {
    using namespace abstractions;
    using namespace abstractions::gui;

    class font : public asset {
        public:

            /**
                * @brief Construct a new font object

                * This function is used to construct a new font object.

                * @param path The path of the font
             */
            font(std::string_view path) noexcept
                : asset("font"), _font()
            {
                this->_font.loadFromFile(std::string(path));
            }

            constexpr const sf::Font &get_font() const noexcept { return this->_font; }

        private:
            sf::Font _font;
    };

    /**
        * @brief Class representing a texture

        * This class is used to represent a texture.
     */
    class texture : public abstractions::gui::texture {
        public:
            texture(std::string_view path) noexcept
                : abstractions::gui::texture("texture"), _texture()
            {
                this->_texture.loadFromFile(std::string(path));
                this->_size = {this->_texture.getSize().x, this->_texture.getSize().y};
            }

            constexpr const sf::Texture &get_texture() const noexcept { return this->_texture; }

            constexpr vector<std::uint32_t> get_size() const noexcept override { return this->_size; }

        private:
            sf::Texture _texture;
            vector<std::uint32_t> _size;
    };

    /**
        * @brief Class representing an asset loader

        * This class is used to represent an asset loader.
     */
    struct asset_loader : public abstractions::gui::asset_loader {
        std::unique_ptr<abstractions::gui::asset> load(std::string_view path, std::string_view type) const noexcept override
        {
            if (type == "font")
                return std::make_unique<font>(path);
            else if (type == "texture")
                return std::make_unique<texture>(path);
            return nullptr;
        }
    };

    /**
        * @brief Class representing a window

        * This class is used to represent a window.
     */
    struct drawable_element : public virtual abstractions::gui::drawable_element {
        drawable_element(sf::Transformable &element, sf::Drawable &drawable) noexcept
            : _element(element), _drawable(drawable)
        {}

        inline operator sf::Drawable &() const noexcept { return this->_drawable; }

        /**
            * @brief Get the position

            * This function is used to get the position.

            * @return The position
         */
        inline vector<float> position() const noexcept override
        {
            return {this->_element.getPosition().x, this->_element.getPosition().y};
        }

        /**
            * @brief Set the position

            * This function is used to set the position.

            * @param position The position
            * @return The old position
         */
        inline vector<float> position(vector<float> position) noexcept override
        {
            const auto old = this->position();
            this->_element.setPosition(position.x, position.y);
            return old;
        }

        /**
            * @brief Get the rotation

            * This function is used to get the rotation.

            * @return The rotation
         */
        inline float rotation() const noexcept override { return this->_element.getRotation(); }
        inline float rotate(float angle) noexcept override
        {
            const auto old = this->rotation();
            this->_element.setRotation(angle);
            return old;
        }

        /**
            * @brief Get the scale

            * This function is used to get the scale.

            * @return The scale
         */
        inline vector<float> scale() const noexcept override
        {
            return {this->_element.getScale().x, this->_element.getScale().y};
        }

        /**
            * @brief Set the scale

            * This function is used to set the scale.

            * @param scale The scale
            * @return The old scale
         */
        inline vector<float> scale(vector<float> scale) noexcept override
        {
            const auto old = this->scale();
            this->_element.setScale(scale.x, scale.y);
            return old;
        }

        /**
            * @brief Get the origin

            * This function is used to get the origin.

            * @return The origin
         */
        inline vector<float> origin() const noexcept override
        {
            return {this->_element.getOrigin().x, this->_element.getOrigin().y};
        }

        /**
            * @brief Set the origin

            * This function is used to set the origin.

            * @param origin The origin
            * @return The old origin
         */
        inline vector<float> set_origin(vector<float> origin) noexcept override
        {
            const auto old = this->origin();
            this->_element.setOrigin(origin.x, origin.y);
            return old;
        }

        /**
            * @brief Get the bounds

            * This function is used to get the bounds.

            * @param local Whether to get the local bounds
            * @return The bounds
         */
        inline void set_outline_color(color color) noexcept override
        {
            if (auto shape = dynamic_cast<sf::Shape *>(&this->_element))
                shape->setOutlineColor(sf::Color(color.r, color.g, color.b, color.a));
        }

        /**
            * @brief Set the outline thickness

            * This function is used to set the outline thickness.

            * @param thickness The thickness
         */
        inline void set_outline_thickness(float thickness) noexcept override
        {
            if (auto shape = dynamic_cast<sf::Shape *>(&this->_element))
                shape->setOutlineThickness(thickness);
        }

        private:
            sf::Transformable &_element;
            sf::Drawable &_drawable;
    };

    /**
        * @brief Class representing a color

        * This class is used to represent a color.
     */
    struct sprite : private sf::Sprite, public drawable_element, public virtual abstractions::gui::sprite {
        sprite(const texture &texture) noexcept
            : Sprite(texture.get_texture()),
            SFML::drawable_element(*this, *this),
            _texture(texture)
        {}

        /**
            * @brief Get the bounds

            * This function is used to get the bounds.

            * @param local Whether to get the local bounds
            * @return The bounds
         */
        inline abstractions::rectangle<float> bounds(bool local = false) const noexcept override
        {
            const auto bounds = local ? this->getLocalBounds() : this->getGlobalBounds();
            return {bounds.left, bounds.top, bounds.width, bounds.height};
        }

        /**
            * @brief Set the texture

            * This function is used to set the texture.

            * @param texture The texture
         */
        inline void set_texture(const abstractions::gui::texture &texture) noexcept override
        {
            this->_texture = dynamic_cast<const class texture &>(texture);
            this->setTexture(this->_texture.get().get_texture());
        }

        /**
            * @brief Get the texture

            * This function is used to get the texture.

            * @return The texture
         */
        inline const abstractions::gui::texture &get_texture() const noexcept override { return this->_texture; }

        /**
            * @brief Set the render area

            * This function is used to set the render area.

            * @param rect The render area
         */
        inline void set_render_area(abstractions::rectangle<std::uint32_t> rect) noexcept override
        {
            this->setTextureRect(sf::IntRect(rect.x, rect.y, rect.width, rect.height));
        }

        private:
            std::reference_wrapper<const texture> _texture;
    };

    /**
        * @brief Class representing an animation

        * This class is used to represent an animation.
     */
    struct animation : public sprite, public abstractions::gui::animation {
        animation(const texture &texture, vector<std::uint32_t> sheet_size,
            std::chrono::steady_clock::duration interval) noexcept
            : SFML::sprite(texture), abstractions::gui::animation(texture, sheet_size, interval)
        {}
    };

    /**

     */
    struct text : private sf::Text, public drawable_element, public abstractions::gui::text {
        text(std::string_view text, const font &font) noexcept
            : Text(std::string(text), font.get_font()),
            SFML::drawable_element(*this, *this),
            _font(font), _content(text)
        {}

        /**
            * @brief Get the bounds

            * This function is used to get the bounds.

            * @param local Whether to get the local bounds
            * @return The bounds
         */
        inline abstractions::rectangle<float> bounds(bool local = false) const noexcept override
        {
            const auto bounds = local ? this->getLocalBounds() : this->getGlobalBounds();
            return {bounds.left, bounds.top, bounds.width, bounds.height};
        }

        /**
            * @brief Set the text

            * This function is used to set the text.

            * @param text The text
         */
        inline void set_text(std::string_view text) noexcept override
        {
            this->_content = std::string(text);
            this->setString(this->_content);
        }
        /**
            * @brief Get the text

            * This function is used to get the text.

            * @return The text
         */
        inline std::string_view get_text() const noexcept override { return this->_content; }

        /**
            * @brief Set the font

            * This function is used to set the font.

            * @param font The font
         */
        inline void set_font(const abstractions::gui::asset &font) noexcept override
        {
            this->_font = dynamic_cast<const class font &>(font);
            this->setFont(this->_font.get().get_font());
        }

        /**
            * @brief Get the font

            * This function is used to get the font.

            * @return The font
         */
        inline const asset &get_font() const noexcept override { return this->_font; }

        /**
            * @brief Set the font size

            * This function is used to set the font size.

            * @param size The font size
         */
        inline void set_font_size(std::uint16_t size) noexcept override { this->setCharacterSize(size); }

        /**
            * @brief Get the font size

            * This function is used to get the font size.

            * @return The font size
         */
        inline std::uint16_t get_font_size() const noexcept override { return this->getCharacterSize(); }

        /**
            * @brief Set the color

            * This function is used to set the color.

            * @param color The color
         */
        inline void set_color(const color &color) noexcept override
        {
            this->setFillColor(sf::Color(color.r, color.g, color.b, color.a));
        }

        /**
            * @brief Get the color

            * This function is used to get the color.

            * @return The color
         */
        inline color get_color() const noexcept override
        {
            const auto color = this->getFillColor();
            return {color.r, color.g, color.b, color.a};
        }

        private:
            std::reference_wrapper<const font> _font;
            std::string _content;
    };

    /**
        * @brief Class representing a shape

        * This class is used to represent a shape.
     */
    struct shape : public drawable_element, public virtual abstractions::gui::shape {
        shape(sf::Shape &shape, element_filling filling) noexcept
            : SFML::drawable_element(shape, shape),
            _shape(shape), _filling(color::transparent)
        {
            this->set_filling(filling);
        }

        /**
            * @brief Get the bounds

            * This function is used to get the bounds.

            * @param local Whether to get the local bounds
            * @return The bounds
         */
        inline abstractions::rectangle<float> bounds(bool local = false) const noexcept override
        {
            const auto bounds = local ? this->_shape.getLocalBounds() : this->_shape.getGlobalBounds();
            return {bounds.left, bounds.top, bounds.width, bounds.height};
        }

        /**
            * @brief Set the filling

            * This function is used to set the filling.

            * @param filling The filling
            * @return The old filling
         */
        inline element_filling set_filling(element_filling filling) noexcept override
        {
            auto old = this->get_filling();
            if (filling.index() == 0) {
                const abstractions::gui::color &color = std::get<0>(filling);
                this->_filling = color;
                this->_shape.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
                this->_shape.setTexture(nullptr);
                return old;
            }
            const texture &texture = dynamic_cast<const class texture &>(std::get<1>(filling).get());
            this->_filling = std::cref(texture);
            this->_shape.setTexture(std::addressof(texture.get_texture()));
            return old;
        }

        /**
            * @brief Get the filling

            * This function is used to get the filling.

            * @return The filling
         */
        inline element_filling get_filling() const noexcept override
        {
            if (this->_filling.index() == 0)
                return std::cref(std::get<0>(this->_filling));
            return std::get<1>(this->_filling);
        }

        private:
            sf::Shape &_shape;
            std::variant<color, std::reference_wrapper<const texture>> _filling;
    };

    /**
        * @brief Class representing a circle

        * This class is used to represent a circle.
     */
    struct circle : private sf::CircleShape, public shape, public abstractions::gui::circle {

        /**
            * @brief Construct a new circle object

            * This function is used to construct a new circle object.

            * @param radius The radius of the circle
            * @param filling The filling of the circle
         */
        circle(float radius, element_filling filling) noexcept
            : CircleShape(radius), SFML::shape(*this, filling)
        {}

        /**
            * @brief Get the radius

            * This function is used to get the radius.

            * @return The radius
         */
        inline float radius() const noexcept override { return this->getRadius(); }

        /**
            * @brief Set the radius

            * This function is used to set the radius.

            * @param radius The radius
            * @return The old radius
         */
        inline float set_radius(float radius) noexcept override
        {
            const auto old = this->getRadius();
            this->setRadius(radius);
            return old;
        }
    };

    /**
        * @brief Class representing a rectangle

        * This class is used to represent a rectangle.
     */
    struct rectangle : private sf::RectangleShape, public shape, public abstractions::gui::rectangle {

        /**
            * @brief Construct a new rectangle object

            * This function is used to construct a new rectangle object.

            * @param size The size of the rectangle
            * @param filling The filling of the rectangle
         */
        rectangle(vector<float> size, element_filling filling) noexcept
            : RectangleShape({size.x, size.y}), SFML::shape(*this, filling)
        {}

        /**
            * @brief Get the size

            * This function is used to get the size.

            * @return The size
         */
        inline vector<float> size() const noexcept override
        {
            const auto size = this->getSize();
            return {size.x, size.y};
        }

        /**
            * @brief Set the size

            * This function is used to set the size.

            * @param size The size
            * @return The old size
         */
        inline vector<float> set_size(vector<float> size) noexcept override
        {
            const auto old = this->size();
            this->setSize({size.x, size.y});
            return old;
        }
    };

    /**
        * @brief Class representing an element factory

        * This class is used to represent an element factory.
     */
    struct element_factory : public abstractions::gui::element_factory {

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param texture The texture
            * @return The element
         */
        virtual std::unique_ptr<abstractions::gui::sprite> make_element(const abstractions::gui::texture &texture)
            const noexcept override
        {
            return std::make_unique<class sprite>(dynamic_cast<const class texture &>(texture));
        }

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param texture The texture
            * @param sheet_size The sheet size
            * @param interval The interval
            * @return The element
         */
        virtual std::unique_ptr<abstractions::gui::animation> make_element(const abstractions::gui::texture &texture,
            vector<std::uint32_t> sheet_size,
            std::chrono::steady_clock::duration interval) const noexcept override
        {
            return std::make_unique<class animation>(dynamic_cast<const class texture &>(texture), sheet_size, interval);
        }

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param text The text
            * @param font The font
            * @param size The size
            * @param color The color
            * @return The element
         */
        virtual std::unique_ptr<abstractions::gui::text> make_element(std::string_view text,
            const asset &font, std::uint16_t size,
            const color &color = color::white) const noexcept override
        {
            const auto &sfml_font = dynamic_cast<const class font &>(font);
            auto text_element = std::make_unique<class text>(text, sfml_font);
            text_element->set_font_size(size);
            text_element->set_color(color);
            return std::move(text_element);
        }

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param radius The radius
            * @param filling The filling
            * @return The element
         */
        virtual std::unique_ptr<abstractions::gui::circle> make_element(float radius,
            element_filling filling) const noexcept override
        {
            return std::make_unique<class circle>(radius, filling);
        }

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param size The size
            * @param filling The filling
            * @return The element
         */
        virtual std::unique_ptr<abstractions::gui::rectangle> make_element(vector<float> size,
            element_filling filling) const noexcept override
        {
            return std::make_unique<class rectangle>(size, filling);
        }
    };

    /**
        * @brief Class representing a window

        * This class is used to represent a window.
     */
    struct window : private sf::RenderWindow, public abstractions::gui::window {
        window(vector<std::uint32_t> size, std::string_view title) noexcept
            : RenderWindow(sf::VideoMode(size.x, size.y), std::string(title))
        {}

        /**
            * @brief Clear the window

            * This function is used to clear the window.
         */
        inline void clear() noexcept override { this->RenderWindow::clear(); }

        /**
            * @brief Draw an element

            * This function is used to draw an element.

            * @param element The element
         */
        inline void draw(const abstractions::gui::drawable_element &element) noexcept override
        {
            const sf::Drawable &drawable = dynamic_cast<const drawable_element &>(element);
            this->RenderWindow::draw(drawable);
        }

        /**
            * @brief Draw a shader background

            * This function is used to draw a shader background.
         */
        inline void draw_shader_background() noexcept override
        {
            static sf::Shader shader;
            static sf::Clock clock;

            static bool shaderLoaded = shader.loadFromFile("assets/shaders/background.frag", sf::Shader::Fragment);
            if (!shaderLoaded) {
                static bool errorLogged = false;
                if (!errorLogged) {
                    std::cerr << "Failed to load shader" << std::endl;
                    errorLogged = true;
                }
                return;
            }

            static sf::RectangleShape fullscreenRect({static_cast<float>(this->getSize().x), static_cast<float>(this->getSize().y)});

            shader.setUniform("iTime", clock.getElapsedTime().asSeconds());
            shader.setUniform("iResolution", sf::Glsl::Vec3(static_cast<float>(this->getSize().x), static_cast<float>(this->getSize().y), 0.0f));

            this->RenderWindow::draw(fullscreenRect, &shader);
        }

        /**
            * @brief Display the window

            * This function is used to display the window.
         */
        inline void display() noexcept override { this->RenderWindow::display(); }

        /**
            * @brief Poll an event

            * This function is used to poll an event.

            * @param event The event
            * @return Whether an event was polled
         */
        inline bool is_open() noexcept override
        {
            sf::Event event;
            while (this->pollEvent(event))
                if (event.type == sf::Event::Closed)
                    this->RenderWindow::close();
            return this->isOpen();
        }

        /**
            * @brief Close the window

            * This function is used to close the window.
         */
        inline void close() noexcept override { this->RenderWindow::close(); }

        /**
            * @brief Get the cursor position

            * This function is used to get the cursor position.

            * @return The cursor position
         */
        inline vector<std::uint32_t> get_cursor_position() const noexcept override
        {
            const auto pos = sf::Mouse::getPosition(*this);
            return {pos.x, pos.y};
        }

        /**
            * @brief Get the size

            * This function is used to get the size.

            * @return The size
         */
        inline vector<std::uint32_t> get_size() const noexcept override
        {
            const auto size = this->getSize();
            return {size.x, size.y};
        }

        /**
            * @brief Check if an input is active

            * This function is used to check if an input is active.

            * @param input The input
            * @return Whether the input is active
         */
        inline bool is_input_active(inputs input) const noexcept override
        {
            if (_mouse_buttons.contains(input))
                return sf::Mouse::isButtonPressed(_mouse_buttons.at(input));
            if (_keyboard_keys.contains(input))
                return sf::Keyboard::isKeyPressed(_keyboard_keys.at(input));
            return false;
        }

        private:
            static const std::unordered_map<abstractions::gui::inputs, sf::Mouse::Button> _mouse_buttons;
            static const std::unordered_map<abstractions::gui::inputs, sf::Keyboard::Key> _keyboard_keys;
    };

    const std::unordered_map<abstractions::gui::inputs, sf::Mouse::Button> window::_mouse_buttons = {
        {abstractions::gui::inputs::lclick, sf::Mouse::Button::Left},
        {abstractions::gui::inputs::rclick, sf::Mouse::Button::Right},
        {abstractions::gui::inputs::mclick, sf::Mouse::Button::Middle},
        {abstractions::gui::inputs::x1click, sf::Mouse::Button::XButton1},
        {abstractions::gui::inputs::x2click, sf::Mouse::Button::XButton2}
    };

    const std::unordered_map<abstractions::gui::inputs, sf::Keyboard::Key> window::_keyboard_keys = {
        {abstractions::gui::inputs::space, sf::Keyboard::Key::Space},
        {abstractions::gui::inputs::enter, sf::Keyboard::Key::Enter},
        {abstractions::gui::inputs::escape, sf::Keyboard::Key::Escape},
        {abstractions::gui::inputs::up, sf::Keyboard::Key::Up},
        {abstractions::gui::inputs::down, sf::Keyboard::Key::Down},
        {abstractions::gui::inputs::left, sf::Keyboard::Key::Left},
        {abstractions::gui::inputs::right, sf::Keyboard::Key::Right},
        {abstractions::gui::inputs::a, sf::Keyboard::Key::A},
        {abstractions::gui::inputs::b, sf::Keyboard::Key::B},
        {abstractions::gui::inputs::c, sf::Keyboard::Key::C},
        {abstractions::gui::inputs::d, sf::Keyboard::Key::D},
        {abstractions::gui::inputs::e, sf::Keyboard::Key::E},
        {abstractions::gui::inputs::f, sf::Keyboard::Key::F},
        {abstractions::gui::inputs::g, sf::Keyboard::Key::G},
        {abstractions::gui::inputs::h, sf::Keyboard::Key::H},
        {abstractions::gui::inputs::i, sf::Keyboard::Key::I},
        {abstractions::gui::inputs::j, sf::Keyboard::Key::J},
        {abstractions::gui::inputs::k, sf::Keyboard::Key::K},
        {abstractions::gui::inputs::l, sf::Keyboard::Key::L},
        {abstractions::gui::inputs::m, sf::Keyboard::Key::M},
        {abstractions::gui::inputs::n, sf::Keyboard::Key::N},
        {abstractions::gui::inputs::o, sf::Keyboard::Key::O},
        {abstractions::gui::inputs::p, sf::Keyboard::Key::P},
        {abstractions::gui::inputs::q, sf::Keyboard::Key::Q},
        {abstractions::gui::inputs::r, sf::Keyboard::Key::R},
        {abstractions::gui::inputs::s, sf::Keyboard::Key::S},
        {abstractions::gui::inputs::t, sf::Keyboard::Key::T},
        {abstractions::gui::inputs::u, sf::Keyboard::Key::U},
        {abstractions::gui::inputs::v, sf::Keyboard::Key::V},
        {abstractions::gui::inputs::w, sf::Keyboard::Key::W},
        {abstractions::gui::inputs::x, sf::Keyboard::Key::X},
        {abstractions::gui::inputs::y, sf::Keyboard::Key::Y},
        {abstractions::gui::inputs::z, sf::Keyboard::Key::Z},
        {abstractions::gui::inputs::one, sf::Keyboard::Key::Num1},
        {abstractions::gui::inputs::two, sf::Keyboard::Key::Num2},
        {abstractions::gui::inputs::three, sf::Keyboard::Key::Num3},
        {abstractions::gui::inputs::four, sf::Keyboard::Key::Num4},
        {abstractions::gui::inputs::five, sf::Keyboard::Key::Num5},
        {abstractions::gui::inputs::six, sf::Keyboard::Key::Num6},
        {abstractions::gui::inputs::seven, sf::Keyboard::Key::Num7},
        {abstractions::gui::inputs::eight, sf::Keyboard::Key::Num8},
        {abstractions::gui::inputs::nine, sf::Keyboard::Key::Num9},
        {abstractions::gui::inputs::zero, sf::Keyboard::Key::Num0},
        {abstractions::gui::inputs::f1, sf::Keyboard::Key::F1},
        {abstractions::gui::inputs::f2, sf::Keyboard::Key::F2},
        {abstractions::gui::inputs::f3, sf::Keyboard::Key::F3},
        {abstractions::gui::inputs::f4, sf::Keyboard::Key::F4},
        {abstractions::gui::inputs::f5, sf::Keyboard::Key::F5},
        {abstractions::gui::inputs::f6, sf::Keyboard::Key::F6},
        {abstractions::gui::inputs::f7, sf::Keyboard::Key::F7},
        {abstractions::gui::inputs::f8, sf::Keyboard::Key::F8},
        {abstractions::gui::inputs::f9, sf::Keyboard::Key::F9},
        {abstractions::gui::inputs::f10, sf::Keyboard::Key::F10},
        {abstractions::gui::inputs::f11, sf::Keyboard::Key::F11},
        {abstractions::gui::inputs::f12, sf::Keyboard::Key::F12},
        {abstractions::gui::inputs::lctrl, sf::Keyboard::Key::LControl},
        {abstractions::gui::inputs::rctrl, sf::Keyboard::Key::RControl},
        {abstractions::gui::inputs::lshift, sf::Keyboard::Key::LShift},
        {abstractions::gui::inputs::rshift, sf::Keyboard::Key::RShift},
        {abstractions::gui::inputs::lalt, sf::Keyboard::Key::LAlt},
        {abstractions::gui::inputs::ralt, sf::Keyboard::Key::RAlt},
        {abstractions::gui::inputs::lsuper, sf::Keyboard::Key::LSystem},
        {abstractions::gui::inputs::rsuper, sf::Keyboard::Key::RSystem},
        {abstractions::gui::inputs::tab, sf::Keyboard::Key::Tab},
        {abstractions::gui::inputs::backtick, sf::Keyboard::Key::Tilde},
        {abstractions::gui::inputs::minus, sf::Keyboard::Key::Dash},
        {abstractions::gui::inputs::equal, sf::Keyboard::Key::Equal},
        {abstractions::gui::inputs::lbracket, sf::Keyboard::Key::LBracket},
        {abstractions::gui::inputs::rbracket, sf::Keyboard::Key::RBracket},
        {abstractions::gui::inputs::backslash, sf::Keyboard::Key::Backslash},
        {abstractions::gui::inputs::semicolon, sf::Keyboard::Key::SemiColon},
        {abstractions::gui::inputs::quote, sf::Keyboard::Key::Quote},
        {abstractions::gui::inputs::comma, sf::Keyboard::Key::Comma},
        {abstractions::gui::inputs::period, sf::Keyboard::Key::Period},
        {abstractions::gui::inputs::slash, sf::Keyboard::Key::Slash},
        {abstractions::gui::inputs::insert, sf::Keyboard::Key::Insert},
        {abstractions::gui::inputs::home, sf::Keyboard::Key::Home},
        {abstractions::gui::inputs::end, sf::Keyboard::Key::End},
        {abstractions::gui::inputs::pageup, sf::Keyboard::Key::PageUp},
        {abstractions::gui::inputs::pagedown, sf::Keyboard::Key::PageDown},
        {abstractions::gui::inputs::pause, sf::Keyboard::Key::Pause},
        {abstractions::gui::inputs::backspace, sf::Keyboard::Key::BackSpace},
        {abstractions::gui::inputs::numpad0, sf::Keyboard::Key::Numpad0},
        {abstractions::gui::inputs::numpad1, sf::Keyboard::Key::Numpad1},
        {abstractions::gui::inputs::numpad2, sf::Keyboard::Key::Numpad2},
        {abstractions::gui::inputs::numpad3, sf::Keyboard::Key::Numpad3},
        {abstractions::gui::inputs::numpad4, sf::Keyboard::Key::Numpad4},
        {abstractions::gui::inputs::numpad5, sf::Keyboard::Key::Numpad5},
        {abstractions::gui::inputs::numpad6, sf::Keyboard::Key::Numpad6},
        {abstractions::gui::inputs::numpad7, sf::Keyboard::Key::Numpad7},
        {abstractions::gui::inputs::numpad8, sf::Keyboard::Key::Numpad8},
        {abstractions::gui::inputs::numpad9, sf::Keyboard::Key::Numpad9}
    };
}
