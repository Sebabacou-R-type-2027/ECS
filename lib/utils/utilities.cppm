module;

#if __cpp_lib_modules < 202207L
#include <any>
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

namespace detail {
    struct type_holder : public std::abstract {
        virtual ~type_holder() noexcept = default;

        virtual constexpr const std::type_info &type() const noexcept = 0;
    };

    template <typename T>
    struct is_in_place_type : std::false_type {};
    template <typename T>
    struct is_in_place_type<std::in_place_type_t<T>> : std::true_type {};

    template <typename T>
    constexpr bool is_in_place_type_v = is_in_place_type<std::remove_cvref_t<T>>::value;
}

export namespace std {
    class unique_any {
        template<typename T>
        struct holder : public detail::type_holder {
            T value;

            template<typename... Args>
            requires std::is_constructible_v<std::decay_t<T>, Args &&...>
            constexpr holder(Args &&...args)
            noexcept(std::is_nothrow_constructible_v<T, Args &&...>)
                : value{std::forward<Args>(args)...}
            {}

            template<typename U, typename... Args>
            requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U> &, Args &&...>
            constexpr holder(std::initializer_list<U> il, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args &&...>)
                : value{il, std::forward<Args>(args)...}
            {}

            constexpr const std::type_info &type() const noexcept final { return typeid(T); }
        };

        std::unique_ptr<detail::type_holder> content;

        template<typename T>
        friend constexpr T *any_cast(unique_any *operand) noexcept;

        public:
            constexpr unique_any() noexcept = default;

            constexpr unique_any(const unique_any &other) noexcept = delete;
            constexpr unique_any(unique_any &&other) noexcept = default;

            template<typename T>
            requires (!std::is_same_v<unique_any, std::decay_t<T>> && !detail::is_in_place_type_v<std::remove_cvref_t<T>>)
            constexpr unique_any(T &&value) noexcept(std::is_nothrow_constructible_v<holder<std::decay_t<T>>, T &&>)
                : content(std::make_unique<holder<std::decay_t<T>>>(std::forward<T>(value)))
            {}

            template<typename T, typename... Args>
            explicit constexpr unique_any(std::in_place_type_t<T>, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<holder<std::decay_t<T>>, Args &&...>)
                : content(std::make_unique<holder<std::decay_t<T>>>(std::forward<Args>(args)...))
            {}

            template<typename T, typename U, typename... Args>
            explicit constexpr unique_any(std::in_place_type_t<T>, std::initializer_list<U> il, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<holder<std::decay_t<T>>, std::initializer_list<U> &, Args &&...>)
                : content(std::make_unique<holder<std::decay_t<T>>>(il, std::forward<Args>(args)...))
            {}

            ~unique_any() noexcept = default;

            constexpr unique_any &operator=(const unique_any &other) noexcept = delete;
            constexpr unique_any &operator=(unique_any &&other) noexcept = default;

            template<typename T> requires (!std::is_same_v<unique_any, std::decay_t<T>>)
            constexpr unique_any &operator=(T &&value) noexcept
            {
                unique_any(std::forward<T>(value)).swap(*this);
                return *this;
            }

            template<typename T, typename... Args>
            constexpr std::decay_t<T> &emplace(Args &&...args)
            noexcept(std::is_nothrow_constructible_v<holder<std::decay_t<T>>, Args &&...>)
            {
                content.swap(std::make_unique(holder<std::decay_t<T>>(std::forward<Args>(args)...)));
                return static_cast<holder<std::decay_t<T>> &>(*content).value;
            }

            template<typename T, typename U, typename... Args>
            constexpr std::decay_t<T> &emplace(std::initializer_list<U> il, Args &&...args)
            noexcept(std::is_nothrow_constructible_v<holder<std::decay_t<T>>, std::initializer_list<U> &, Args &&...>)
            {
                content.swap(std::make_unique(holder<std::decay_t<T>>(il, std::forward<Args>(args)...)));
                return static_cast<holder<std::decay_t<T>> &>(*content).value;
            }

            constexpr void reset() noexcept { content.reset(); }
            constexpr void swap(unique_any &other) noexcept { content.swap(other.content); }

            constexpr bool has_value() const noexcept { return content != nullptr; }
            constexpr const std::type_info &type() const noexcept { return content ? content->type() : typeid(void); }
    };

    constexpr void swap(unique_any &lhs, unique_any &rhs) noexcept { lhs.swap(rhs); }

    template<typename T>
    constexpr T *any_cast(unique_any *operand) noexcept
    {
        static_assert(!std::is_void_v<T>);
        return operand && operand->type() == typeid(T)
            ? std::addressof(static_cast<unique_any::holder<std::decay_t<T>> &>(*operand->content).value)
            : nullptr;
    }

    template<typename T>
    constexpr const T *any_cast(const unique_any *operand) noexcept
    {
        static_assert(!std::is_void_v<T>);
        return any_cast<T>(const_cast<unique_any *>(operand));
    }

    template<typename T>
    constexpr T any_cast(unique_any &operand)
    {
        using U = std::remove_cvref_t<T>;
        static_assert(std::is_constructible_v<T, U &>);
        if(typeid(T) != operand.type())
            throw std::bad_any_cast();
        return static_cast<T>(*std::any_cast<U>(std::addressof(operand)));
    }

    template<typename T>
    constexpr T any_cast(const unique_any &operand)
    {
        using U = std::remove_cvref_t<T>;
        static_assert(std::is_constructible_v<T, const U &>);
        if(typeid(T) != operand.type())
            throw std::bad_any_cast();
        return static_cast<T>(*std::any_cast<U>(std::addressof(operand)));
    }

    template<typename T>
    constexpr T any_cast(unique_any&& operand)
    {
        using U = std::remove_cvref_t<T>;
        static_assert(std::is_constructible_v<T, U>);
        if(typeid(T) != operand.type())
            throw std::bad_any_cast();
        return static_cast<T>(std::move(*std::any_cast<U>(std::addressof(operand))));
    }

    template<typename T, typename... Args>
    constexpr unique_any make_any(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<unique_any, std::in_place_type_t<T>, Args...>)
    {
        return unique_any(std::in_place_type<T>, std::forward<Args>(args)...);
    }

    template<typename T, typename U, typename... Args>
    constexpr unique_any make_any(std::initializer_list<U> il, Args &&...args)
    noexcept(std::is_nothrow_constructible_v<unique_any, std::in_place_type_t<T>, std::initializer_list<U>, Args...>)
    {
        return unique_any(std::in_place_type<T>, il, std::forward<Args>(args)...);
    }
}
