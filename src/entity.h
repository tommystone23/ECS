#ifndef ENTITY_H
#define ENTITY_H
#include <stdint.h>
#include "component.h"

class ECS;

class Entity
{
public:
    friend class ECS;
    Entity(entity_id id, ECS *ecs);
    ~Entity(){}
    entity_id get_id() { return _entity_id; }
    component_record_t* component_record() { return &_component_record; }
    std::vector<component_data_t>& get_component_cache() { return _components; }
private:
    ECS *_ecs;
    entity_id _entity_id;
    component_record_t _component_record;
    // Data to be added will be held here until committed
    std::vector<component_data_t> _components;

    void add_component(component_data_t comp_data);
    void commit_components(const std::vector<component_grouping_t*> &component_groupings);

    void free_component_cache();
};

#endif // ENTITY_H