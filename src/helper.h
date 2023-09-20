#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <algorithm>
#include "component.h"

template <class C>
inline bool contains(const std::vector<C> &v, C &c) 
{
    if (std::find(v.begin(), v.end(), c) != v.end())
        return true;
    return false;
}

bool contains_components(std::vector<component_id> &types, std::vector<component_id> &types2);
bool exclusive_contains_components(std::vector<component_id> &types, std::vector<component_id> &types2);

#endif // HELPER_H