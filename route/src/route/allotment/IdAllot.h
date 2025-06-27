#pragma once
#include "route/route.h"

namespace route
{
    template <typename TElement> class ROUTE_API IdAllot;
}
using namespace route;

template <typename TElement>
using IdAllot_T = IdAllot<TElement>;

template <typename TElement>
class ROUTE_API route::IdAllot
{
private:
    /* Maximum *ize is 1024 */
    uint16_t size;
    // Allotted size indicates the size allocated by alloc
    // The value is a power of 2 with the minimum being 4
    uint16_t allottedSize;

    TElement** elems;

public:
    IdAllot ();
    ~IdAllot ();
    uint16_t getSize (void);
    bool setSize (uint32_t);
    TElement* getElem (uint32_t);
};

