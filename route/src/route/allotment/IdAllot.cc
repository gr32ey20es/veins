#include "IdAllot.h"

IdAllot::IdAllot()
{
    this->count = 0;
}

uint8_t IdAllot::getCount (void)
{
    return this->count;
}

void IdAllot::setCount (uint8_t newCount)
{
    this->count = newCount;
}
