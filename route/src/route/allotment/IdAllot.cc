#include "IdAllot.h"

template <typename TElement>
IdAllot<TElement>::IdAllot ()
{
    size = 0;
    allottedSize = 0;

    elems = NULL;
}

template <typename TElement>
IdAllot<TElement>::~IdAllot ()
{
    uint16_t id;

    while (size)
    {
        id = size - 1;
        delete elems[id];
        --size;
    }

    std::free (elems);
}

template <typename TElement>
uint16_t IdAllot<TElement>::getSize (void)
{
    return size;
}

template <typename TElement>
bool IdAllot<TElement>::setSize (uint32_t newSize)
{
    ASSERT (newSize <= 1024);

    uint32_t scale, id;
    uint64_t sizeB;
    TElement** newElems;

    if (newSize > allottedSize)
    {
        // Determine proper size
        scale = 4;
        while (scale < newSize)
            scale <<= 1;
        sizeB = scale * sizeof (TElement*);

        // Allocate, size changing from allottedSize to scale
        if (elems == NULL)
        {
            elems = (TElement**) std::malloc (sizeB);
            if (elems == NULL)
                return false;
        }
        else
        {
            newElems = (TElement **) std::realloc (elems, sizeB);
            if (newElems == NULL)
                return false;
            else
                elems = newElems;
        }

        // Initialize newly allocated elements
        id = allottedSize;
        while (id < scale)
            elems[id++] = NULL;
        allottedSize = scale;
    }

    if (newSize < size)
    {
        id = size - 1;
        while (newSize <= id)
            elems[id--] = NULL;
    }
    size = newSize;

    return true;
}

template <typename TElement>
TElement* IdAllot<TElement>::getElem (uint32_t id)
{
    ASSERT (id < size);
    
    if (elems[id] == NULL)
        elems[id] = new TElement();

    return elems[id];
}
