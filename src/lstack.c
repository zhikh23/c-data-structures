#include "lstack.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"


struct lstack_node;
typedef struct lstack_node lstack_node_t;

struct lstack_node {
    sval_t val;
    lstack_node_t *next;
};

typedef struct {
    const istack_t *class;
    lstack_node_t *top;
} lstack_t;

_Static_assert(offsetof(lstack_t, class) == 0);


void *lstack_ctor(void *_class, va_list *ap) {
    lstack_t *self = _class;
    self->top = NULL;
    return self;
}

void lstack_dtor(void *_self) {
    lstack_t *self = _self;

    lstack_node_t *node = self->top, *next = (node) ? node->next : NULL;
    while (node != NULL) {
        free(node);
        node = next;
        next = (node) ? node->next : NULL;
    }

    self->top = NULL;
}

int lstack_empty(const void *_self) {
    const lstack_t *self = _self;
    return self->top == NULL;
}

int lstack_push(void *_self, const sval_t v) {
    lstack_t *self = _self;

    lstack_node_t *node = malloc(sizeof(lstack_node_t));
    if (node == NULL)
        return ENOMEM;

    node->val = v;
    node->next = self->top;
    self->top = node;

    return 0;
}

sval_t lstack_peek(const void *_self) {
    const lstack_t *self = _self;
    assert(self->top != NULL);
    return self->top->val;
}

sval_t lstack_pop(void *_self) {
    lstack_t *self = _self;
    assert(self->top != NULL);
    const sval_t v = self->top->val;
    lstack_node_t *next = self->top->next;
    free(self->top);
    self->top = next;
    return v;
}

const istack_t ListStackClass = {
    .size     = sizeof(lstack_t),
    .ctor     = lstack_ctor,
    .dtor     = lstack_dtor,
    .is_empty = lstack_empty,
    .push     = lstack_push,
    .peek     = lstack_peek,
    .pop      = lstack_pop,
};
