#include "astack.h"

#include <assert.h>

#define ASTACK_INITIAL_CAPACITY 16

typedef sval_t sval;
#define T sval
#include "vec.h"
#undef T


typedef struct {
    const istack_t *class;
    vec_sval_t *arr;
} astack_t;

_Static_assert(offsetof(astack_t, class) == 0);


void *astack_ctor(void *_class, va_list *ap) {
    astack_t *self = _class;

    size_t cap = va_arg(*ap, size_t);
    if (cap == 0)
        cap = ASTACK_INITIAL_CAPACITY;

    void *tmp = vec_sval_create(cap);
    if (tmp == NULL)
        return ERR_PTR(-ENOMEM);
    self->arr = tmp;

    return self;
}

void astack_dtor(void *_self) {
    astack_t *self = _self;
    free(self->arr);
    self->arr = NULL;
}

int astack_empty(const void *_self) {
    const astack_t *self = _self;
    return self->arr->len == 0;
}

int astack_push(void *_self, const sval_t v) {
    astack_t *self = _self;
    void *tmp = vec_sval_push(self->arr, v);
    if (IS_ERR(tmp))
        return PTR_ERR(tmp);
    self->arr = tmp;
    return 0;
}

sval_t astack_peek(const void *_self) {
    const astack_t *self = _self;
    assert(self->arr->len > 0);
    return vec_sval_entry(self->arr, self->arr->len - 1);
}

sval_t astack_pop(void *_self) {
    astack_t *self = _self;
    assert(self->arr->len > 0);
    const sval_t v = vec_sval_entry(self->arr, self->arr->len - 1);
    self->arr = vec_sval_pop(self->arr);
    return v;
}

const istack_t ArrayStackClass = {
    .size     = sizeof(astack_t),
    .ctor     = astack_ctor,
    .dtor     = astack_dtor,
    .is_empty = astack_empty,
    .push     = astack_push,
    .peek     = astack_peek,
    .pop      = astack_pop,
};
