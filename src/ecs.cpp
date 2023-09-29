#include "ecs.h"
#include "system.h"
#include <functional>

ECS::ECS()
{}

ECS::~ECS()
{
    delete _thread_pool;
    for(auto i = _entities.begin(); i != _entities.end(); ++i) {
        delete i->second;
    }
    _entities.clear();

    for(component_grouping_t *grouping : _component_groupings) {
        free_component_data(grouping->component_data);
    }   
}

void ECS::init()
{
    _thread_pool = new ThreadPool();
}

entity_id ECS::create_entity()
{
    Entity *entity = new Entity(_id_pool++, this);
    entity_id id = entity->get_id();
    _entities.insert(std::pair(id, entity));

    return id;
}

size_t ECS::get_component_size(component_id id)
{
    if(_component_sizes.find(id) == _component_sizes.end()) 
        return -1;
    
    return _component_sizes.at(id);
}

bool ECS::get_groupings_by_type(std::vector<component_id> &types, 
                                std::vector<component_grouping_t*> *grouping_out)
{
    std::vector<component_grouping_t*> groupings;
    for(component_grouping_t *grouping : _component_groupings) {
        if(contains_components(types, grouping->types))
            groupings.push_back(grouping);
    }

    if(!groupings.size()) 
        return false;

    if(grouping_out)
        *grouping_out = groupings;

    return true;
}

bool ECS::get_groupings_by_type_exclusive(std::vector<component_id> &types, 
                                            component_grouping_t *grouping_out)
{
    component_grouping_t *new_grouping = NULL;
    for(component_grouping_t *grouping : _component_groupings) {
        if(exclusive_contains_components(grouping->types, types)) {
            new_grouping = grouping;
            break; 
        }
    }

    if(!new_grouping)
        return false;

    if(grouping_out)
        grouping_out = new_grouping;

    return true;
}

void ECS::add_component_grouping(component_grouping_t *grouping)
{
    // Check if component grouping already exists
    if(contains<component_grouping_t*>(_component_groupings, grouping))
        return;

    _component_groupings.push_back(grouping);
}

void ECS::remove_component_grouping(component_grouping_t *grouping)
{
    for(component_grouping_t *grouping_ : _component_groupings) {
        if(grouping_ == grouping) {
            free_component_data(grouping->component_data);
            delete grouping;
        }
    }
}

void ECS::commit_components(entity_id id)
{
    Entity *entity = _entities[id];
    entity->commit_components(_component_groupings);
}

void ECS::register_system(System *system, int layer)
{
    // Init systems layers
    if(_systems.size() == 0) 
        _systems.resize(1);
    if(layer >= _systems.size())
        _systems.resize(layer + 1);

    _systems[layer].push_back(system);
}

void ECS::run_systems(float delta_time)
{
    for(std::vector<System*> &system_layer : _systems) {
        if(system_layer.size() <= 0)
            continue;
        if(system_layer.size() > 1) {
            run_concurrent_systems(delta_time, system_layer);
        }
        else {
            system_layer[0]->run_system(delta_time);
        }
    }
}

void ECS::run_concurrent_systems(float delta_time, std::vector<System*> systems)
{
    for(System *system : systems) {
        std::function<void(float)> job = std::bind(&System::run_system, system, std::placeholders::_1);
        _thread_pool->queue_job(job, delta_time);
    }
}
