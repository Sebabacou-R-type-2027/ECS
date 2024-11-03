export module ecs:abstractions.gui;
import :abstractions;

import std;

using namespace std::chrono_literals;
export namespace ecs::abstractions::gui {

    /**
        * @brief Enumeration of the inputs

        * This enumeration is used to define the inputs of the GUI.
     */
    enum class inputs {
        left, right, up, down,
        escape, enter,
        backspace, del,
        space,
        a, b, c, d, e, f, g, h, i, j, k, l, m,
        n, o, p, q, r, s, t, u, v, w, x, y, z,
        one, two, three, four, five, six, seven, eight, nine, zero,
        f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
        lctrl, rctrl, lshift, rshift, lalt, ralt, lsuper, rsuper,
        tab, backtick, minus, equal, lbracket, rbracket, backslash, semicolon, quote, comma, period, slash,
        insert, home, end, pageup, pagedown, pause,
        lclick, rclick, mclick, x1click, x2click,
        numpad0, numpad1, numpad2, numpad3, numpad4, numpad5, numpad6, numpad7, numpad8, numpad9
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct color {
        /**
            * @brief Construct a new color object

            * This function is used to construct a new color object.

            * @param red The red value
            * @param green The green value
            * @param blue The blue value
            * @param alpha The alpha value
         */
        constexpr color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 255) noexcept
            : r(red), g(green), b(blue), a(alpha)
        {}

        std::uint8_t r, g, b, a;

        static const color black, white,
            red, green, blue,
            yellow, cyan, magenta,
            transparent;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    const color color::black(0, 0, 0), color::white(255, 255, 255),
        color::red(255, 0, 0), color::green(0, 255, 0), color::blue(0, 0, 255),
        color::yellow(255, 255, 0), color::cyan(0, 255, 255), color::magenta(255, 0, 255),
        color::transparent(0, 0, 0, 0);

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct asset {
        constexpr asset(std::string_view type) noexcept : _type(type) {};

        virtual constexpr ~asset() noexcept = 0;

        constexpr std::string_view get_type() const noexcept { return this->_type; }

        private:
            const std::string _type;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    constexpr asset::~asset() noexcept = default;

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct texture : public asset {
        using asset::asset;

        constexpr virtual vector<std::uint32_t> get_size() const noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct asset_loader {
        virtual ~asset_loader() noexcept = default;

        virtual std::unique_ptr<asset> load(std::string_view path, std::string_view type) const noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct drawable_element {
        using repositioner = void(drawable_element &element, vector<float> position);
        virtual ~drawable_element() noexcept = default;

        /**
            * @brief Get the position

            * This function is used to get the position.

            * @return The position
         */
        virtual vector<float> position() const noexcept = 0;

        /**
            * @brief Set the position

            * This function is used to set the position.

            * @param position The position
            * @return The position
         */
        virtual vector<float> position(vector<float> position) noexcept = 0;

        /**
            * @brief Get the rotation

            * This function is used to get the rotation.

            * @return The rotation
         */
        virtual float rotation() const noexcept = 0;

        /**
            * @brief Rotate the element

            * This function is used to rotate the element.

            * @param angle The angle
            * @return The angle
         */
        virtual float rotate(float angle) noexcept = 0;

        /**
            * @brief Get the scale

            * This function is used to get the scale.

            * @return The scale
         */
        virtual vector<float> scale() const noexcept = 0;

        /**
            * @brief Set the scale

            * This function is used to set the scale.

            * @param scale The scale
            * @return The scale
         */
        virtual vector<float> scale(vector<float> scale) noexcept = 0;

        /**
            * @brief Get the origin

            * This function is used to get the origin.

            * @return The origin
         */
        virtual vector<float> origin() const noexcept = 0;
        
        /**
            * @brief Set the origin

            * This function is used to set the origin.

            * @param origin The origin
            * @return The origin
         */
        virtual vector<float> set_origin(vector<float> origin) noexcept = 0;

        /**
            * @brief Get the bounds

            * This function is used to get the bounds.

            * @param local The local flag
            * @return The bounds
         */
        virtual rectangle<float> bounds(bool local = false) const noexcept = 0;

        /**
            * @brief Get the outline color

            * This function is used to get the outline color.

            * @return The outline color
         */
        virtual void set_outline_color(color color) noexcept = 0;

        /**
            * @brief Set the outline thickness

            * This function is used to set the outline thickness.

            * @param thickness The thickness
         */
        virtual void set_outline_thickness(float thickness) noexcept = 0;

        /**
            * @brief Get the visibility

            * This function is used to get the visibility.

            * @return The visibility
         */
        constexpr bool is_visible() const noexcept { return _visible; }

        /**
            * @brief Set the visibility

            * This function is used to set the visibility.

            * @param visible The visibility
            * @return The visibility
         */
        constexpr bool make_visible(bool visible) noexcept
        {
            bool old = _visible;
            _visible = visible;
            return old;
        }

        /**
            * @brief Get the repositioning

            * This function is used to get the repositioning.

            * @return The repositioning
         */
        inline void set_repositioning(std::optional<std::function<repositioner>> reposition) noexcept
        {
            _reposition = std::move(reposition);
        }

        /**
            * @brief Reposition the element

            * This function is used to reposition the element.

            * @param position The position
         */
        constexpr void reposition(vector<float> position) noexcept
        {
            if (_reposition)
                (*_reposition)(*this, position);
            else
                this->position(position);
        }

        /**
            * @brief Reposition the element to the center

            * This function is used to reposition the element to the center.

            * @param element The element
            * @param position The position
         */
        static constexpr void reposition_center(drawable_element &element, vector<float> position) noexcept
        {
            const auto bounds = element.bounds(true);
            element.position({position.x + bounds.width / 2, position.y + bounds.height / 2});
        }

        private:
            bool _visible = true;
            std::optional<std::function<repositioner>> _reposition;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct sprite : public virtual drawable_element {
        virtual ~sprite() noexcept = default;

        /**
            * @brief Set the texture

            * This function is used to set the texture.

            * @param texture The texture
         */
        virtual void set_texture(const texture &texture) noexcept = 0;

        /**
            * @brief Get the texture

            * This function is used to get the texture.

            * @return The texture
         */
        virtual const texture &get_texture() const noexcept = 0;

        /**
            * @brief Set the render area

            * This function is used to set the render area.

            * @param rect The render area
         */
        virtual void set_render_area(rectangle<std::uint32_t> rect) noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    class animation : public virtual sprite {
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
            animation(const texture &texture, vector<std::uint32_t> sheet_size,
                std::chrono::steady_clock::duration length) noexcept
                : frame_columns(sheet_size.x), frame_lines(sheet_size.y),
                _image_width(texture.get_size().x / sheet_size.x), _image_height(texture.get_size().y / sheet_size.y),
                frame_length(length)
            {}

            /**
                * @brief Update the animation

                * This function is used to update the animation.

                * @param delta The delta
             */
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

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct text : public virtual drawable_element {
        virtual ~text() noexcept = default;

        /**
            * @brief Set the text

            * This function is used to set the text.

            * @param text The text
         */
        virtual void set_text(std::string_view text) noexcept = 0;

        /**
            * @brief Get the text

            * This function is used to get the text.

            * @return The text
         */
        virtual std::string_view get_text() const noexcept = 0;

        /**
            * @brief Set the font

            * This function is used to set the font.

            * @param font The font
         */
        virtual void set_font(const asset &font) noexcept = 0;

        /**
            * @brief Get the font

            * This function is used to get the font.

            * @return The font
         */
        virtual const asset &get_font() const noexcept = 0;

        /**
            * @brief Set the font size

            * This function is used to set the font size.

            * @param size The font size
         */
        virtual void set_font_size(std::uint16_t size) noexcept = 0;

        /**
            * @brief Get the font size

            * This function is used to get the font size.

            * @return The font size
         */
        virtual std::uint16_t get_font_size() const noexcept = 0;

        /**
            * @brief Set the color

            * This function is used to set the color.

            * @param color The color
         */
        virtual void set_color(const color &color) noexcept = 0;

        /**
            * @brief Get the color

            * This function is used to get the color.

            * @return The color
         */
        virtual color get_color() const noexcept = 0;
    };

    using element_filling = std::variant<std::reference_wrapper<const color>, std::reference_wrapper<const texture>>;

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct shape : public virtual drawable_element {
        virtual ~shape() noexcept = default;

        virtual element_filling get_filling() const noexcept = 0;
        virtual element_filling set_filling(element_filling filling) noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct circle : public virtual shape {
        virtual ~circle() noexcept = default;

        virtual float radius() const noexcept = 0;
        virtual float set_radius(float radius) noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct rectangle : public virtual shape {
        virtual ~rectangle() noexcept = default;

        virtual vector<float> size() const noexcept = 0;
        virtual vector<float> set_size(vector<float> size) noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct element_factory {
        virtual ~element_factory() noexcept = default;

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param texture The texture
            * @return The element
         */
        virtual std::unique_ptr<sprite> make_element(const texture &texture) const noexcept = 0;

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param texture The texture
            * @param sheet_size The sheet size
            * @param interval The interval
            * @return The element
         */
        virtual std::unique_ptr<animation> make_element(const texture &texture,
            vector<std::uint32_t> sheet_size,
            std::chrono::steady_clock::duration interval = 33ms) const noexcept = 0;

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param text The text
            * @param font The font
            * @param size The size
            * @param color The color
            * @return The element
         */
        virtual std::unique_ptr<text> make_element(std::string_view text,
            const asset &font, std::uint16_t size, const color &color = color::white) const noexcept = 0;

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param radius The radius
            * @param filling The filling
            * @return The element
         */
        virtual std::unique_ptr<circle> make_element(float radius,
            element_filling filling = color::white) const noexcept = 0;

        /**
            * @brief Make an element

            * This function is used to make an element.

            * @param size The size
            * @param filling The filling
            * @return The element
         */
        virtual std::unique_ptr<rectangle> make_element(vector<float> size,
            element_filling filling = color::white) const noexcept = 0;
    };

    /**
        * @brief Enumeration of the mouse buttons

        * This enumeration is used to define the mouse buttons of the GUI.
     */
    struct window {
        virtual ~window() noexcept = default;

        virtual void clear() noexcept = 0;
        virtual void draw(const drawable_element &element) noexcept = 0;
        virtual void draw_shader_background() noexcept = 0;
        virtual void display() noexcept = 0;

        virtual bool is_open() noexcept = 0;
        virtual void close() noexcept = 0;

        virtual vector<std::uint32_t> get_cursor_position() const noexcept = 0;
        virtual vector<std::uint32_t> get_size() const noexcept = 0;

        virtual bool is_input_active(inputs input) const noexcept = 0;
    };
}
