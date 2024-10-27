# Getting Started

## The registry

The core class of this ECS library is the `ecs::registry` class.
It is the main class that you will interact with to create and manage entities and components.

### Entities and components

#### Creating an entity

To create an entity, you can call the `ecs::registry::create_entity` method.
This method will return an `ecs::entity` object that you can use to interact with the entity.

```cpp
ecs::registry registry;

auto entity = registry.create_entity();
```

An `ecs::entity` object is an opaque handle to an entity, though it is convertible to a unique `std::size_t` identifier.

#### Creating components

A component is simply an arbitrary object that you can attach to an entity.
A single component of any given type (as per the `typeid()` of the type) can be attached to an entity at a time.

To attach a component to an entity, you can call the `ecs::registry::emplace_component` or `ecs::registry::add_component` methods.

```cpp
struct position {
    constexpr position(float x, float y) noexcept : x(x), y(y) {}

    float x;
    float y;
};

ecs::registry registry;

using namespace std::chrono_literals;
ecs::systems::position_logger logger(registry, 0s);

auto entity = registry.create_entity();
registry.run_systems(); // No output, no entity has a position component

registry.emplace_component<position>(entity, 1.0f, 2.0f);
registry.run_systems(); // Output: "Entity #0: Position: (1.0, 2.0)"

registry.add_component<position>(entity, position(3.0f, 4.0f));
registry.run_systems(); // Output: "Entity #0: Position: (3.0, 4.0)"
```

#### Removing components

To remove a component from an entity, you can call the `ecs::registry::remove_component` template method.

```cpp
ecs::registry registry;

using namespace std::chrono_literals;
ecs::systems::position_logger logger(registry, 0s);

auto entity = registry.create_entity();
registry.emplace_component<ecs::components::position>(entity, 1.0f, 2.0f);
registry.run_systems(); // Output: "Entity #0: Position: (1.0, 2.0)"

registry.remove_component<ecs::components::position>(entity);
registry.run_systems(); // No output, no entity has a position component
```

#### Destroying entities

To destroy an entity, you can call the `ecs::registry::erase_entity` method.

```cpp
ecs::registry registry;

using namespace std::chrono_literals;
ecs::systems::position_logger logger(registry, 0s);

auto entity = registry.create_entity();
registry.emplace_component<ecs::components::position>(entity, 1.0f, 2.0f);
registry.run_systems(); // Output: "Entity #0: Position: (1.0, 2.0)"

registry.erase_entity(entity);
registry.run_systems(); // No output, no entity in the registry
```

Erasing an entity from the registry will also call the destroy all the components attached to that entity.

### Systems

Systems are the main way to interact with entities and components in the registry.
They are invocable objects that are called on all entities matching their required component types.

Formally, a system is a invocable object that takes an arbitrary number of component references as arguments.
A system can optionally take the `ecs::entity` it was called onto and/or a reference to the underlying `ecs::entity_container` containing the latter.

#### Registering systems

To register a system, you can call the `ecs::registry::register_system` template method, using the component types the system takes.

```cpp
struct position {
    constexpr position(float x, float y) noexcept : x(x), y(y) {}

    float x;
    float y;
};

struct velocity : public position {
    using position::position;
};

ecs::registry registry;

// The system uses the position and velocity components,
// the ecs::entity parameter is not used in the template since it is handled using overload resolution
registry.register_system<position, const velocity>([](ecs::entity entity, position& pos, const velocity& vel) {
    std::cout << "Entity #" << entity << " moved from (" << pos.x << ", " << pos.y << ") to ";
    pos.x += vel.x;
    pos.y += vel.y;
    std::cout << "(" << pos.x << ", " << pos.y << ")" << std::endl;
});

auto entity = registry.create_entity();
registry.emplace_component<position>(entity, 1.0f, 2.0f);
registry.emplace_component<velocity>(entity, 1.0f, 2.0f);

registry.run_systems(); // Output: "Entity #0 moved from (1.0, 2.0) to (2.0, 4.0)"
```

#### Running systems

A system is called on all entities that have all the required components.
Entities are processed in an arbitrary order, only the systems are guaranteed to be called in the inverse order in which they were registered.

Entities missing a required component are simply skipped by the system.

```cpp
ecs::registry registry;

using namespace std::chrono_literals;
ecs::systems::position_logger logger(registry, 0s);
registry.register_system<>([](ecs::entity entity) {
    std::cout << "Entity #" << entity << " is alive" << std::endl;
});

auto first_entity = registry.create_entity();
auto second_entity = registry.create_entity();
registry.emplace_component<ecs::components::position>(second_entity, 1.0f, 2.0f);

registry.run_systems();
/* Possible output:
 * Entity #0 is alive
 * Entity #1 is alive
 * Entity #1: Position: (1.0, 2.0)"
 */
```

## Example usage

```cpp
#include <csignal> // Signal macros

import std;
import ecs;

using namespace ecs;

bool running = true;

static void stop(int)
{
    running = false;
}

int main()
{
    registry registry;

    // Create an entity with basic movement components
    auto entity = registry.create_entity();
    registry.emplace_component<components::position>(entity);
    registry.emplace_component<components::engine::velocity>(entity, 1.0f, 2.0f);

    // Register a system that logs the position of entities every second
    systems::position_logger logger(registry);
    // Register the default movement system
    registry.register_system<components::postion, const components::engine::velocity>(systems::engine::movement);

    // Run the systems 20 times per second
    signal(SIGINT, stop);
    while (running) {
        using namespace std::chrono_literals;
        auto next = std::chrono::steady_clock::now() + 50ms;
        registry.run_systems();
        std::this_thread::sleep_until(next);
    }

    return 0;
}
```
