#include "map.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "err.h"

void *map_new(const imap_t *class, ...) {
    void *p = malloc(class->size);
    if (p == NULL)
        return ERR_PTR(-ENOMEM);

    *(const imap_t **)p = class;
    if (class->ctor) {
        va_list ap;
        va_start(ap, class->ctor);
        p = class->ctor(p, &ap);
        va_end(ap);
    }

    return p;
}

void map_destroy(void *self) {
    const imap_t *const *cp = self;
    assert(self && *cp);

    if ((*cp)->dtor)
        (*cp)->dtor(self);

    free(self);
}

void map_insert(void *self, const mkey_t key, const mval_t value) {
    const imap_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->insert);

    (*cp)->insert(self, key, value);
}

map_res_t map_lookup(const void *self, const mkey_t key) {
    const imap_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->lookup);

    return (*cp)->lookup(self, key);
}

int map_remove(void *self, const mkey_t key) {
    const imap_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->remove);

    return (*cp)->remove(self, key);
}
