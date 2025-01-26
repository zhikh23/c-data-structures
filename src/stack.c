#include "stack.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"

void *stack_new(const istack_t *class, ...) {
    void *p = malloc(class->size);
    if (p == NULL)
        return ERR_PTR(-ENOMEM);

    *(const istack_t **)p = class;
    if (class->ctor) {
        va_list ap;
        va_start(ap, class);
        p = class->ctor(p, &ap);
        va_end(ap);
    }

    return p;
}

void stack_destroy(void *self) {
    const istack_t *const *cp = self;
    assert(self && *cp);

    if ((*cp)->dtor)
        (*cp)->dtor(self);

    free(self);
}

int stack_empty(const void *self) {
    const istack_t *const *cp = self;
    assert(self && *cp);

    return (*cp)->is_empty(self);
}

int stack_push(void *self, const sval_t val) {
    const istack_t *const *cp = self;
    assert(self && *cp && (*cp)->push);

    return (*cp)->push(self, val);
}

sval_t stack_peek(const void *self) {
    const istack_t *const *cp = self;
    assert(self && *cp && (*cp)->peek);

    return (*cp)->peek(self);
}

sval_t stack_pop(void *self) {
    const istack_t *const *cp = self;
    assert(self && *cp && (*cp)->pop);

    return (*cp)->pop(self);
}
