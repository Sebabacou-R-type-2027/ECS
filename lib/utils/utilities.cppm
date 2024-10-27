#if __cpp_lib_modules < 202207L
module;

#include <initializer_list>
#include <memory>
#include <typeinfo>
#include <type_traits>
#endif
export module utils:utilities;
import :types;

#if __cpp_lib_modules >= 202207L
import std;
#endif

struct type_holder : public std::abstract {
    virtual ~type_holder() noexcept = default;

    virtual constexpr const std::type_info &type() const noexcept = 0;
};

export namespace std {
    class unique_any {
        template<typename T>
        struct holder : public type_holder {
            std::decay_t<T> value;

            template<typename... Args>
            constexpr holder(Args &&...args)
            noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, Args...>)
                : value(std::forward<Args>(args)...)
            {}

            template<typename U, typename... Args>
            constexpr holder(std::initializer_list<U> il, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, std::initializer_list<U>, Args...>)
                : value(il, std::forward<Args>(args)...)
            {}

            constexpr const std::type_info &type() const noexcept final { return typeid(std::decay_t<T>); }
        };

        std::unique_ptr<type_holder> content;

        template<typename T>
        friend constexpr T *any_cast(unique_any *operand) noexcept;

        public:
            constexpr unique_any() noexcept = default;

            constexpr unique_any(const unique_any &other) noexcept = delete;
            constexpr unique_any(unique_any &&other) noexcept = default;

            template<typename T>// requires (!std::is_same_v<unique_any, typename std::decay_t<T>>)
            unique_any(T &&value)
                : content(std::make_unique<holder<T>>(std::forward<T>(value)))
            {}

            template<typename T, typename... Args>
            explicit constexpr unique_any(std::in_place_type_t<T>, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<holder<T>, Args...>)
                : content(std::make_unique<holder<T>>(std::forward<Args>(args)...))
            {}

            template<typename T, typename U, typename... Args>
            explicit constexpr unique_any(std::initializer_list<U> il, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<holder<T>, std::initializer_list<U>, Args...>)
                : content(std::make_unique<holder<T>>(il, std::forward<Args>(args)...))
            {}

            ~unique_any() noexcept = default;

            constexpr unique_any &operator=(const unique_any &other) noexcept = delete;
            constexpr unique_any &operator=(unique_any &&other) noexcept = default;

            template<typename T>
            constexpr unique_any &operator=(T &&value)
            {
                unique_any(std::forward<T>(value)).swap(*this);
                return *this;
            }

            template<typename T, typename... Args>
            typename std::decay_t<T> &emplace(Args &&...args) {
                content.swap(std::make_unique(holder<T>(std::forward<Args>(args)...)));
                return static_cast<holder<T> &>(*content).value;
            }

            template<typename T, typename U, typename... Args>
            typename std::decay_t<T> &emplace(std::initializer_list<U> il, Args &&...args) {
                content.swap(std::make_unique(holder<T>(il, std::forward<Args>(args)...)));
                return static_cast<holder<T> &>(*content).value;
            }

            constexpr void reset() noexcept { content.reset(); }
            constexpr void swap(unique_any &other) noexcept { content.swap(other.content); }

            constexpr bool has_value() const noexcept { return content != nullptr; }
            constexpr const std::type_info &type() const noexcept { return content ? content->type() : typeid(void); }
    };

    inline void swap(unique_any &lhs, unique_any &rhs) noexcept { lhs.swap(rhs); }

    template<typename T>
    constexpr T *any_cast(unique_any *operand) noexcept
    {
        return operand && operand->type() == typeid(T)
            ? std::addressof(static_cast<typename unique_any::holder<T> &>(*operand->content).value)
            : nullptr;
    }

    template<typename T>
    constexpr const T *any_cast(const unique_any *operand) noexcept
    {
        return any_cast<T>(const_cast<unique_any *>(operand));
    }

    template<typename T>
    constexpr T any_cast(unique_any &operand)
    {
        using V = typename std::remove_reference_t<T>;

        V *result = any_cast<V>(std::addressof(operand));
        if(!result)
            throw std::bad_any_cast();

        return static_cast<typename std::conditional_t<std::is_reference_v<T>, T, T&>>(*result);
    }

    template<typename T>
    constexpr T any_cast(const unique_any &operand) noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return any_cast<const T &>(const_cast<unique_any &>(operand));
    }

    template<typename T> requires std::is_rvalue_reference_v<T &&> || std::is_const_v<typename std::remove_reference_t<T>>
    constexpr T any_cast(unique_any&& operand) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        return std::move(any_cast<T &>(operand));
    }
}
