#include "component.h"
#include "ecs.h"
#include <stdio.h>
#include <string.h>

void create_new_grouping(Entity *entity, ECS *ecs)
{
    entity->component_record()->component_group = new component_grouping_t();
    entity->component_record()->index = 0;
    component_grouping_t *grouping = entity->component_record()->component_group;
    grouping->entities.push_back(entity->get_id());

    std::vector<component_data_t> new_components = entity->get_component_cache();
    for(component_data_t comp_data : new_components) {
        unsigned char *comp_buffer = (unsigned char*)(malloc(comp_data.data_size * MAX_ENTITIES + sizeof(component_header_t)));
        memset(comp_buffer, 0x00, comp_data.data_size * MAX_ENTITIES + sizeof(component_header_t));
        entity->component_record()->component_group->component_data.push_back(comp_buffer);

        unsigned char *p = comp_buffer;
        component_header_t header;
        header.type = comp_data.id;
        header.bytes_written = comp_data.data_size;
        memcpy(p, &header, sizeof(component_header_t));
        p += sizeof(component_header_t);

        memcpy(p, comp_data.data, comp_data.data_size);

        grouping->entities.push_back(entity->get_id());
        grouping->types.push_back(comp_data.id);
    }

    ecs->add_component_grouping(grouping);
}

void add_component_data(std::vector<unsigned char*> &dest_comp_data, 
                                std::vector<component_data_t> &src_comp_data,
                                ECS *ecs) 
{
    for(unsigned char *data : dest_comp_data) {
        component_header_t *header = (component_header_t*)data;
        int comp_id = header->type;
        size_t data_size = ecs->get_component_size(comp_id);
        int bytes_written = header->bytes_written;

        unsigned char *p = data;
        p += sizeof(component_header_t) + bytes_written;
        
        component_data_t src;
        for(const component_data_t &comp_data : src_comp_data) {
            if(comp_data.id == comp_id) {
                src = comp_data;
                break;
            }
        }

        memcpy(p, src.data, src.data_size);

        header->bytes_written += data_size;
    }
}

void free_component_data(std::vector<unsigned char*> &comp_data)
{
    for(unsigned char *data : comp_data) {
        free(data);
    }

    comp_data.clear();
}
