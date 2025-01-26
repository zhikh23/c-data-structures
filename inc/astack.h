/**
 * astack.h - реализация стека на массиве (векторе).
 */
#ifndef ASTACK_H
#define ASTACK_H

#include "stack.h"

extern const istack_t ArrayStackClass;
// map_new(ArrayStack, capacity)
static const istack_t *ArrayStack = &ArrayStackClass;

#endif // ASTACK_H
