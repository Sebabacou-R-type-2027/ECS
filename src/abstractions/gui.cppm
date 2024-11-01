module;

#if __cpp_lib_modules < 202207L
#include <cstdint>
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#endif
export module ecs:abstractions.gui;
import :abstractions;

#if __cpp_lib_modules >= 202207L
import std;
#endif

using namespace std::chrono_literals;
export namespace ecs::abstractions::gui {
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
        lclick, rclick, mclick, x1click, x2click
    };

    struct color {
        constexpr color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 255) noexcept
            : r(red), g(green), b(blue), a(alpha)
        {}

        std::uint8_t r, g, b, a;

        static const color black, white,
            red, green, blue,
            yellow, cyan, magenta,
            transparent;
    };

    const color color::black(0, 0, 0), color::white(255, 255, 255),
        color::red(255, 0, 0), color::green(0, 255, 0), color::blue(0, 0, 255),
        color::yellow(255, 255, 0), color::cyan(0, 255, 255), color::magenta(255, 0, 255),
        color::transparent(0, 0, 0, 0);

    struct asset {
        constexpr asset(std::string_view type) noexcept : _type(type) {};

        virtual constexpr ~asset() noexcept = 0;

        constexpr std::string_view get_type() const noexcept { return this->_type; }

        private:
            const std::string _type;
    };

    constexpr asset::~asset() noexcept = default;

    struct texture : public asset {
        using asset::asset;

        constexpr virtual vector<std::uint32_t> get_size() const noexcept = 0;
    };

    struct asset_loader {
        virtual ~asset_loader() noexcept = default;

        virtual std::unique_ptr<asset> load(std::string_view path, std::string_view type) const noexcept = 0;
    };

    struct drawable_element {
        using repositioner = void(drawable_element &element, vector<float> position);
        virtual ~drawable_element() noexcept = default;

        virtual vector<float> position() const noexcept = 0;
        virtual vector<float> position(vector<float> position) noexcept = 0;

        virtual float rotation() const noexcept = 0;
        virtual float rotate(float angle) noexcept = 0;

        virtual vector<float> scale() const noexcept = 0;
        virtual vector<float> scale(vector<float> scale) noexcept = 0;

        virtual vector<float> origin() const noexcept = 0;
        virtual vector<float> set_origin(vector<float> origin) noexcept = 0;

        virtual rectangle<float> bounds(bool local = false) const noexcept = 0;

        constexpr bool is_visible() const noexcept { return _visible; }
        constexpr bool make_visible(bool visible) noexcept
        {
            bool old = _visible;
            _visible = visible;
            return old;
        }

        inline void set_repositioning(std::optional<std::function<repositioner>> reposition) noexcept
        {
            _reposition = std::move(reposition);
        }
        constexpr void reposition(vector<float> position) noexcept
        {
            if (_reposition)
                (*_reposition)(*this, position);
            else
                this->position(position);
        }

        static constexpr void reposition_center(drawable_element &element, vector<float> position) noexcept
        {
            const auto bounds = element.bounds(true);
            element.position({position.x + bounds.width / 2, position.y + bounds.height / 2});
        }

        private:
            bool _visible = true;
            std::optional<std::function<repositioner>> _reposition;
    };

    struct sprite : public virtual drawable_element {
        virtual ~sprite() noexcept = default;

        virtual void set_texture(const texture &texture) noexcept = 0;
        virtual const texture &get_texture() const noexcept = 0;

        virtual void set_render_area(rectangle<std::uint32_t> rect) noexcept = 0;
    };

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

    struct text : public virtual drawable_element {
        virtual ~text() noexcept = default;

        virtual void set_text(std::string_view text) noexcept = 0;
        virtual std::string_view get_text() const noexcept = 0;

        virtual void set_font(const asset &font) noexcept = 0;
        virtual const asset &get_font() const noexcept = 0;

        virtual void set_font_size(std::uint16_t size) noexcept = 0;
        virtual std::uint16_t get_font_size() const noexcept = 0;

        virtual void set_color(const color &color) noexcept = 0;
        virtual color get_color() const noexcept = 0;
    };

    using element_filling = std::variant<std::reference_wrapper<const color>, std::reference_wrapper<const texture>>;

    struct shape : public virtual drawable_element {
        virtual ~shape() noexcept = default;

        virtual element_filling get_filling() const noexcept = 0;
        virtual element_filling set_filling(element_filling filling) noexcept = 0;
    };

    struct circle : public virtual shape {
        virtual ~circle() noexcept = default;

        virtual float radius() const noexcept = 0;
        virtual float set_radius(float radius) noexcept = 0;
    };

    struct rectangle : public virtual shape {
        virtual ~rectangle() noexcept = default;

        virtual vector<float> size() const noexcept = 0;
        virtual vector<float> set_size(vector<float> size) noexcept = 0;
    };

    struct element_factory {
        virtual ~element_factory() noexcept = default;

        virtual std::unique_ptr<sprite> make_element(const texture &texture) const noexcept = 0;

        virtual std::unique_ptr<animation> make_element(const texture &texture,
            vector<std::uint32_t> sheet_size,
            std::chrono::steady_clock::duration interval = 33ms) const noexcept = 0;

        virtual std::unique_ptr<text> make_element(std::string_view text,
            const asset &font, std::uint16_t size, const color &color = color::white) const noexcept = 0;

        virtual std::unique_ptr<circle> make_element(float radius,
            element_filling filling = color::white) const noexcept = 0;

        virtual std::unique_ptr<rectangle> make_element(vector<float> size,
            element_filling filling = color::white) const noexcept = 0;
    };

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
