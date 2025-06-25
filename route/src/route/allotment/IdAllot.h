#pragma once
#include "route/route.h"

namespace route { class IdAllot; }
using namespace route;

class ROUTE_API route::IdAllot
{
private:
    uint8_t count;

public:
    IdAllot();
    uint8_t getCount (void);
    void setCount (uint8_t);
};

