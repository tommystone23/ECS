#ifndef ID_GENERATOR
#define ID_GENERATOR
#include <stdint.h>

using IDType = uint32_t;

/*
    Will generate a unique ID for each type
*/

template<class T>
class IdGenerator
{
public:
    template<class U>
    static const IDType get_new_id()
    {
        static const IDType id_counter = _count++;
        return id_counter;
    }

private:
    static IDType _count;
};

#endif // ID_GENERATOR

template<class T> IDType IdGenerator<T>::_count = 0;