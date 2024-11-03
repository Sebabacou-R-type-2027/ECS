export module ecs:core;

import std;
import utils;

/**
 * @brief Entity Component System (ECS) core namespace
 */
export namespace ecs {
    /**
     * @brief Opaque type representing an entity
     */
    struct entity {
        /**
         * @brief Convert the entity to its underlying unique identifier in the @ref entity_container
         *
         * @return std::size_t
         */
        constexpr operator std::size_t() const noexcept { return _id; }

        private:
            constexpr entity(std::size_t id) noexcept : _id(id) {}
            friend class entity_container;

            std::size_t _id;
    };

    class entity_container;

    struct scene {
        constexpr scene(entity_container &ec) noexcept
            : _ec(ec), _entities()
        {}

        virtual ~scene() noexcept;

        protected:
            entity_container &_ec;
            std::vector<entity> _entities;

            virtual void create_entities() noexcept = 0;

        private:
            friend class entity_container;
    };

    /**
     * @brief Class managing entities and their components
     */
    class entity_container {
        using component_container = std::unordered_map<std::type_index, std::unique_any>;
        using entity_components = std::unordered_map<std::size_t, component_container>;

        entity_components _components;
        std::vector<bool> _entities;
        std::unique_ptr<scene> _scene;
        std::size_t _nextEntity = 0;

        static constexpr entity to_entity(std::size_t id) noexcept { return entity(id); }
        public:

            entity_container() noexcept = default;
            entity_container(const entity_container &) = delete;

            void begin_scene(std::unique_ptr<scene> scene) noexcept
            {
                _scene = std::move(scene);
                if (_scene)
                    _scene->create_entities();
            }

            ~entity_container() noexcept { _scene.reset(); }

            /**
             * @brief Exception thrown when an entity is not found
             */
            struct entity_not_found_error : public std::runtime_error {
                explicit entity_not_found_error(entity to) noexcept
                    : std::runtime_error(std::format("Entity #{} does not exist", static_cast<std::size_t>(to)))
                {};
            };

            /**
             * @brief Create a new entity in the @ref entity_container using the next available identifier
             *
             * @return entity The new entity
             */
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

            /**
             * @brief Get the @ref entity associated with the given identifier
             *
             * @param id The identifier of the entity
             * @return std::optional<entity> The entity if it exists, otherwise std::nullopt
             */
            constexpr std::optional<entity> get_entity(std::size_t id) const noexcept
            {
                if (id >= _entities.size() || !_entities.at(id))
                    return std::nullopt;
                return entity(id);
            }

            /**
             * @brief Add a component to an @ref entity
             *
             * @tparam Component The type of the component
             * @param to The entity to add the component to
             * @param component The component to add
             * @return Component& The added component
             * @throw entity_not_found_error If the entity does not exist
             */
            template<typename Component>
            constexpr Component &add_component(entity to, Component &&component)
            {
                if (!get_entity(to))
                    throw entity_not_found_error(to);
                return std::any_cast<Component &>(_components[to]
                    .try_emplace(typeid(Component), std::forward<Component>(component)).first->second);
            }

            /**
             * @brief Emplace a component to an @ref entity
             *
             * @tparam Component The type of the component
             * @tparam Args The types of the arguments to forward to the component constructor
             * @param to The entity to add the component to
             * @param args The arguments to forward to the component constructor
             * @return Component& The added component
             * @throw entity_not_found_error If the entity does not exist
             */
            template<typename Component, typename... Args>
            constexpr Component &emplace_component(entity to, Args &&...args)
            {
                if (!get_entity(to))
                    throw entity_not_found_error(to);
                _components[to].erase(typeid(Component));
                return std::any_cast<Component &>(_components.at(to)
                    .try_emplace(typeid(Component), std::in_place_type<Component>, std::forward<Args>(args)...).first->second);
            }

            /**
             * @brief Get a specific component from an @ref entity
             *
             * @remarks If the entity does not exist, @ref std::nullopt is returned
             * @tparam Component The type of the component to obtain
             * @param from The entity to get the component from
             * @return std::optional<std::reference_wrapper<Component>> A reference to the component if it exists on the @ref entity, otherwise @ref std::nullopt
             */
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

            /**
             * @brief Get a specific component from an @ref entity
             *
             * @remarks If the entity does not exist, @ref std::nullopt is returned
             * @tparam Component The type of the component to obtain
             * @param from The entity to get the component from
             * @return std::optional<std::reference_wrapper<const Component>> A const-reference to the component if it exists on the @ref entity, otherwise @ref std::nullopt
             */
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

            /**
             * @brief Remove a specific component from an @ref entity
             *
             * @tparam Component The type of the component to remove
             * @param from The entity to remove the component from
             * @return bool @ref true if the component was removed, otherwise @ref false
             */
            template<typename Component>
            constexpr bool remove_component(entity from) noexcept
            {
                if (!get_entity(from))
                    return false;
                return _components[from].erase(typeid(Component));
            }

            /**
             * @brief Erase an @ref entity from the @ref entity_container and destroy all its associated components
             *
             * @param e The entity to erase
             * @return bool @ref true if the entity was erased, otherwise @ref false
             * @remarks The internal identifier of the entity is made available for new entities
             */
            inline bool erase_entity(entity e) noexcept
            {
                _entities.at(e) = false;
                return _components.erase(e);
            }

            /**
             * @brief Get a view of all the entities in the @ref entity_container
             *
             * @return decltype(auto) A view of all the entities
             */
            constexpr auto get_entities() const noexcept
            {
                return std::ranges::to<std::vector<entity>>(_components | std::views::keys | std::views::transform(to_entity));
            }
    };

    class system;

    /**
     * @brief @ref entity_container with systems logic
     */
    class registry : public entity_container {
        private:
            std::forward_list<system> _systems;

        public:
            /**
             * @brief Register a new system to run on all entities
             *
             * @tparam Args The types of the components the system requires
             * @param f The invocable object to execute by passing the components
             * @return constexpr const system& The registered system
             */
            template<typename... Args>
            constexpr const system &register_system(auto &&f) noexcept;

            /**
             * @brief Remove a system from the registry
             *
             * @param system The system to remove
             * @return bool @ref true if the system was removed, otherwise @ref false
             */
            inline bool remove_system(const system &system) noexcept
            {
                return _systems.remove_if([&system](const auto &s){ return std::addressof(s) == std::addressof(system); });
            }

            /**
             * @brief Run all systems on all entities
             */
            constexpr void run_systems() noexcept;

            void register_gui_systems() noexcept;
            void register_engine_systems() noexcept;
    };

    /**
     * @brief Opaque type representing code to be run on all entities
     */
    class system {
        /**
         * @brief Create a new system that runs on entities with the specified components
         *
         * @tparam Args The types of the components the system requires
         * @param f The invocable object to execute by passing the components
         */
        template<typename... Args, std::invocable<std::add_lvalue_reference_t<Args>...> Function>
        constexpr system(Function &&f, std::typeset_t<Args...>) noexcept;
        /**
         * @brief Create a new system that runs on entities with a reference to the @ref entity_container and the specified components
         *
         * @tparam Args The types of the components the system requires
         * @param f The invocable object to execute by passing the @ref entity_container and the components
         */
        template<typename... Args, std::invocable<entity_container &, std::add_lvalue_reference_t<Args>...> Function>
        constexpr system(Function &&f, std::typeset_t<Args...>) noexcept;
        /**
         * @brief Create a new system that runs on entities with their @ref entity object and the specified components
         *
         * @tparam Args The types of the components the system requires
         * @param f The invocable object to execute by passing the entity's @ref entity object and the components
         */
        template<typename... Args, std::invocable<entity, std::add_lvalue_reference_t<Args>...> Function>
        constexpr system(Function &&f, std::typeset_t<Args...>) noexcept;
        /**
         * @brief Create a new system that runs on entities with their @ref entity object, a reference to the @ref entity_container
           and the specified components
         *
         * @tparam Args The types of the components the system requires
         * @param f The invocable object to execute by passing the entity's @ref entity object, the @ref entity_container and the components
         */
        template<typename... Args, std::invocable<entity, entity_container &, std::add_lvalue_reference_t<Args>...> Function>
        constexpr system(Function &&f, std::typeset_t<Args...>) noexcept;

        /**
         * @brief Invoke the system's code on the entity if it has the required components
         *
         * @tparam Function The type of the invocable object
         * @tparam Args The types of the parameters the system requires
         * @param f The invocable object to execute
         * @param on The entity to run the system on
         * @param ec The @ref entity_container to query the components from
         */
        template<typename Function, typename... Args>
        static constexpr void invoke(Function &&f, entity on, entity_container &ec, std::typeset_t<Args...>) noexcept;

        template<typename... Args>
        friend constexpr const system &registry::register_system(auto &&f) noexcept;

        public:
            system(const system &) = delete;
            system(system &&) noexcept = default;

            /**
             * @brief Dynamically-constructed function to run the system on an entity
             *
             * @param ec The @ref entity_container to query the components from
             * @param e The entity to run the system on
             */
            std::function<void(entity_container &, entity)> update;
    };

    scene::~scene() noexcept
    {
        for (auto e : _entities)
            _ec.erase_entity(e);
    }

    constexpr void registry::run_systems() noexcept
    {
        std::ranges::for_each(_systems, [this](const system &system) constexpr noexcept {
            std::ranges::for_each(this->get_entities(), [this, &system](auto e) constexpr noexcept {
                system.update(*this, e);
            });
        });
    };

    template<typename... Args>
    constexpr const system &registry::register_system(auto &&f) noexcept
    {
        _systems.emplace_front(system(std::forward<decltype(f)>(f), std::typeset<Args...>));
        return _systems.front();
    }

    template<typename Function, typename... Args>
    constexpr void system::invoke(Function &&f, entity on, entity_container &ec, std::typeset_t<Args...>) noexcept
    {
        if (!(ec.get_entity_component<Args>(on) && ...))
            return;
        std::invoke(std::forward<Function>(f), ec.get_entity_component<Args>(on)->get()...);
    }

    template<typename... Args, std::invocable<std::add_lvalue_reference_t<Args>...> Function>
    constexpr system::system(Function &&f, std::typeset_t<Args...>) noexcept
        : update([&f](entity_container &ec, entity e) constexpr noexcept {
            invoke(std::forward<Function>(f), e, ec, std::typeset<Args...>);
        })
    {}

    template<typename... Args, std::invocable<entity_container &, std::add_lvalue_reference_t<Args>...> Function>
    constexpr system::system(Function &&f, std::typeset_t<Args...>) noexcept
        : update([&f](entity_container &ec, entity e) constexpr noexcept {
            invoke(std::bind_front(std::forward<Function>(f), std::ref(ec)), e, ec, std::typeset<Args...>);
        })
    {}

    template<typename... Args, std::invocable<entity, std::add_lvalue_reference_t<Args>...> Function>
    constexpr system::system(Function &&f, std::typeset_t<Args...>) noexcept
        : update([&f](entity_container &ec, entity e) constexpr noexcept {
            invoke(std::bind_front(std::forward<Function>(f), e), e, ec, std::typeset<Args...>);
        })
    {}

    template<typename... Args, std::invocable<entity, entity_container &, std::add_lvalue_reference_t<Args>...> Function>
    constexpr system::system(Function &&f, std::typeset_t<Args...>) noexcept
        : update([&f](entity_container &ec, entity e) constexpr noexcept {
            invoke(std::bind_front(std::forward<Function>(f), e, std::ref(ec)), e, ec, std::typeset<Args...>);
        })
    {}
}
