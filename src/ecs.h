#ifndef ECS_H
#define ECS_H

#define MAX_ENTITIES 10000

#include <stdint.h>
#include <unordered_map>
#include "entity.h"
#include "idgenerator.h"
#include <vector>
#include "helper.h"
#include <string.h>
#include "threadpool.h"

using entity_id = uint32_t;

class System;

class ECS
{
public:
    ECS();
    ~ECS();

    void init();
    entity_id create_entity();
    Entity* get_entity(entity_id ent_id) { return _entities[ent_id]; }
    size_t get_component_size(component_id id);
    bool get_groupings_by_type(std::vector<component_id> &types, 
                                                            std::vector<component_grouping_t*> *grouping_out); // Get grouping if has the types
    bool get_groupings_by_type_exclusive(std::vector<component_id> &types, 
                                                                        component_grouping_t *grouping_out);  // Gets the grouping if it ONLY has the types provided

    void add_component_grouping(component_grouping_t *grouping);
    void remove_component_grouping(component_grouping_t *grouping);

    template <class C> component_id get_component_id() {
        return IdGenerator<Component>::get_new_id<C>();
    }

    template <class C> component_id add_component(entity_id entity_id, C &&c) {
        component_id comp_id = IdGenerator<Component>::get_new_id<C>();
        if(_component_sizes.find(comp_id) == _component_sizes.end())
            _component_sizes.insert(std::pair(comp_id, sizeof(C)));

        Entity *entity = _entities.at(entity_id);

        C *data = (C*)malloc(sizeof(C));
        memcpy(data, &c, sizeof(C));
        component_data_t comp_data = { comp_id, (unsigned char*)data, sizeof(C) };
        entity->add_component(comp_data);

        return comp_id;
    }

    void commit_components(entity_id id);

    void register_system(System *system, int layer);
    void run_systems(float delta_time);

private:
    std::unordered_map<entity_id, Entity*> _entities;
    entity_id _id_pool;

    std::vector<component_grouping_t*> _component_groupings;
    std::unordered_map<component_id, size_t> _component_sizes;

    std::vector<std::vector<System*>> _systems;

    ThreadPool *_thread_pool;

    void run_concurrent_systems(float delta_time, std::vector<System*> systems);
};

#endif // ECS_H
