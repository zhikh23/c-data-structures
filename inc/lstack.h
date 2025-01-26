/**
 * astack.h - реализация стека на односвязанном списке.
 */
#ifndef LSTACK_H
#define LSTACK_H

#include "stack.h"

extern const istack_t ListStackClass;
// map_new(ListStack)
static const istack_t *ListStack = &ListStackClass;

#endif // LSTACK_H
