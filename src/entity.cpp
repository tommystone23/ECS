#include "entity.h"
#include "helper.h"
#include <stdio.h>

Entity::Entity(entity_id id, ECS *ecs)
    : _ecs(ecs),
    _entity_id(id)
{}

void Entity::add_component(component_data_t comp_data)
{
    _components.push_back(comp_data);
}

void Entity::commit_components(const std::vector<component_grouping_t*> &component_groupings)
{
    if(!_component_record.component_group) {
        // Check if another component grouping exists that has all our types

        // Group comp_id's into vector
        std::vector<component_id> comp_types;
        for(const component_data_t &comp : _components) {
            comp_types.push_back(comp.id);
        }

        component_grouping_t *grouping = NULL;
        for(component_grouping_t *comp_group : component_groupings) {
            if(exclusive_contains_components(comp_group->types, comp_types)) {
                grouping = comp_group;
                break;
            }
        }

        // Found grouping with same comp id's
        if(grouping) {
            add_component_data(grouping->component_data, _components, _ecs);
            _component_record.component_group = grouping;
            _component_record.index = grouping->entities.size() ;
            grouping->entities.push_back(_entity_id);
            free_component_cache();
        }
        // Create new grouping
        else {
            create_new_grouping(this, _ecs);
            free_component_cache();
        }
    }
}

void Entity::free_component_cache()
{
    for(component_data_t &comp_data : _components) {
        free(comp_data.data);
    }

    _components.clear();
}
