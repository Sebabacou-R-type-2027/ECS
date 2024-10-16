export module ecs;

import std;

export namespace ecs {
    struct entity {
        constexpr operator std::size_t() const noexcept { return _id; }

        private:
            constexpr entity(std::size_t id) noexcept : _id(id) {}
            friend class entity_container;

            std::size_t _id;
    };

    class entity_container {
        public:
            using component_container = std::unordered_map<std::type_index, std::any>;
            using entity_components = std::unordered_map<std::size_t, component_container>;

        private:
            static const component_container _emptyComponents;
            entity_components _components;
            std::vector<bool> _entities;
            std::size_t _nextEntity = 0;

        protected:
            entity_container() noexcept = default;

        public:
            entity_container(const entity_container &) = delete;

            struct entity_not_found_error : public std::runtime_error {
                explicit entity_not_found_error(entity to) noexcept
                    : std::runtime_error(std::format("Entity #{} does not exist", static_cast<std::size_t>(to)))
                {};
            };

            constexpr entity create_entity() noexcept
            {
                auto nextEntity = std::find(_entities.begin(), _entities.end(), false);
                if (nextEntity == _entities.end()) {
                    _entities.emplace_back(true);
                    return _nextEntity++;
                }
                *nextEntity = true;
                return std::distance(_entities.begin(), nextEntity);
            }

            constexpr std::optional<entity> get_entity(std::size_t id) const noexcept
            {
                if (id >= _entities.size() || !_entities.at(id))
                    return std::nullopt;
                return entity(id);
            }

            template<typename Component>
            constexpr Component &add_component(entity to, Component &&component)
            {
                if (!get_entity(to))
                    throw entity_not_found_error(to);
                return std::any_cast<Component &>(_components[to]
                    .insert_or_assign(typeid(Component), std::forward<Component>(component)).first->second);
            }

            template<typename Component, typename... Args>
            constexpr Component &emplace_component(entity to, Args &&...args)
            {
                if (!get_entity(to))
                    throw entity_not_found_error(to);
                _components[to].erase(typeid(Component));
                return std::any_cast<Component &>(_components.at(to)
                    .try_emplace(typeid(Component), Component(std::forward<Args>(args)...)).first->second);
            }

            template<typename Component>
            constexpr std::optional<std::reference_wrapper<Component>> get_entity_component(entity from) noexcept
            {
                if (!get_entity(from))
                    return std::nullopt;
                auto &components = _components[from];
                return components.contains(typeid(Component))
                    ? std::make_optional(std::ref(std::any_cast<Component &>(components.at(typeid(Component)))))
                    : std::nullopt;
            }

            template<typename Component>
            constexpr std::optional<std::reference_wrapper<const Component>> get_entity_component(entity from) const noexcept
            {
                if (!get_entity(from))
                    return std::nullopt;
                if (!_components.contains(from))
                    return std::nullopt;
                auto &components = _components.at(from);
                return components.contains(typeid(Component))
                    ? std::make_optional(std::cref(std::any_cast<const Component &>(components.at(typeid(Component)))))
                    : std::nullopt;
            }

            constexpr component_container &get_components(entity from)
            {
                if (!get_entity(from))
                    throw entity_not_found_error(from);
                return _components[from];
            }

            constexpr const component_container &get_components(entity from) const noexcept
            {
                if (!get_entity(from))
                    return _emptyComponents;
                if (_components.contains(from))
                    return _components.at(from);
                return _emptyComponents;
            }

            template<typename Component>
            constexpr auto get_components() noexcept
            {
                return _components | std::views::values | std::views::filter([](component_container &components){
                    return components.contains(typeid(Component))
                        && components.at(typeid(Component)).type() == typeid(Component);
                }) | std::views::transform([](component_container &components){
                    return std::any_cast<Component &>(components.at(typeid(Component)));
                });
            }

            template<typename Component>
            constexpr auto get_components() const noexcept
            {
                return _components | std::views::values | std::views::filter([](const component_container &components){
                    return components.contains(typeid(Component))
                        && components.at(typeid(Component)).type() == typeid(Component);
                }) | std::views::transform([](component_container &components){
                    return std::any_cast<const Component &>(components.at(typeid(Component)));
                });
            }

            template<typename Component>
            constexpr bool remove_component(entity from) noexcept
            {
                if (!get_entity(from))
                    return false;
                return _components[from].erase(typeid(Component));
            }

            constexpr bool erase_entity(entity to) noexcept
            {
                _entities.at(to) = false;
                return _components.erase(to);
            }

            constexpr decltype(auto) get_entities() const noexcept
            {
                return _components | std::views::keys | std::views::transform(to_entity);
            }

        private:
            static constexpr entity to_entity(std::size_t id) noexcept { return entity(id); }
    };

    const entity_container::component_container entity_container::_emptyComponents;
}
