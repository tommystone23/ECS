#include "helper.h"

bool exclusive_contains_components(std::vector<component_id> &types, 
                                        std::vector<component_id> &types2)
{
    if(types.size() != types2.size())
        return false;
    
    std::sort(types.begin(), types.end());
    std::sort(types2.begin(), types2.end());

    return types == types2;
}