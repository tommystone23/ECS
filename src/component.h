#ifndef COMPONENT_H
#define COMPONENT_H
#include <vector>
#include <set>
#include <stdint.h>
#include <stdlib.h>

class ECS;
class Entity;

#define MAX_ENTITIES 10000

using entity_id = uint32_t;
using component_id = uint16_t;

class Component{}; // Simply used to create unique component IDs with templates

typedef struct component_data_s {
    component_id id;
    unsigned char *data;
    int data_size;
} component_data_t;

// Will be stored at the beginning of component data in grouping
typedef struct component_header_s {
    component_id type;
    int bytes_written;
} component_header_t;

// Holds a group of component data that an entity has
typedef struct component_grouping_s {
    std::vector<component_id> types; // Vector of component IDs contained in grouping
    // Vector of raw byte data for components, component_header_t at the beginning of each component_data
    std::vector<unsigned char*> component_data;
    std::vector<entity_id> entities; // Entity IDs in the grouping
} component_grouping_t;

// Used by entity to find it's component grouping and the index in the grouping
typedef struct component_record_s {
    component_grouping_t *component_group = NULL;
    int index = -1;
} component_record_t;

void create_new_grouping(Entity *entity, ECS *ecs);

void add_component_data(std::vector<unsigned char*> &dest_comp_data, 
                                std::vector<component_data_t> &src_comp_data,
                                ECS *ecs);

void free_component_data(std::vector<unsigned char*> &comp_data);


#endif // COMPONENT_H
