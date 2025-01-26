#include "shmap.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "err.h"
#include "hash.h"

#define SHMAP_DEFAULT_INITIAL_CAPACITY 16
#define SHMAP_GROW_FACTOR 2
#define SHMAP_HEIGHT_THRESHOLD_TO_GROW 3


struct hlist;
typedef struct hlist hlist_t;

struct hlist {
    mut_mkey_t key;
    mval_t     value;
    hlist_t   *next;
};

typedef struct {
    const imap_t *class;
    size_t        cap;
    hlist_t     **heads;
    hash_t        seed;
    hash_func_t   hash;
} shmap_t;

_Static_assert(offsetof(shmap_t, class) == 0);


#define shmap_heads_for_each(self, head)                                   \
    for (head = (self)->heads; head < (self)->heads + (self)->cap; head++)

#define hlist_for_each(head, node) \
    for (node = (head); node; node = node->next)

#define hlist_for_each_safe(head, node, n) \
    for (node = (head), n = (head)->next;  \
         node;                             \
         node = n, n = n ? n->next : NULL)

hlist_t *hlist_new(mkey_t key, const mval_t value) {
    hlist_t *self = malloc(sizeof(hlist_t));
    if (self == NULL)
        return ERR_PTR(-ENOMEM);

    self->key = strdup(key);
    self->value = value;
    self->next = NULL;

    return self;
}

hlist_t *hlist_insert_head(hlist_t *head, mkey_t key, mval_t value) {
    hlist_t *node = hlist_new(key, value);
    if (IS_ERR(node))
        return ERR_CAST(node);
    node->next = head;
    return node;
}

size_t hlist_count(const hlist_t *head) {
    size_t count = 0;
    for (const hlist_t *node = head; node; node = node->next)
        count++;
    return count;
}

hlist_t *hlist_lookup(hlist_t *head, mkey_t key) {
    hlist_t *node;
    hlist_for_each(head, node) {
        if (STR_EQ(node->key, key))
            return node;
    }
    return NULL;
}

hlist_t *hlist_delete(hlist_t *head, mkey_t key) {
    if (head == NULL)
        return NULL;

    if (STR_EQ(head->key, key)) {
        hlist_t *next = head->next;
        free(head->key);
        free(head);
        return next;
    }

    hlist_t *prev = head;
    hlist_t *node;
    hlist_for_each(head->next, node) {
        if (STR_EQ(node->key, key)) {
            prev->next = node->next;
            free(node->key);
            free(node);
            return head;
        }
        prev = node;
        node = node->next;
    }

    return head;
}

void *shmap_ctor(void *_class, va_list *ap) {
    shmap_t *self = _class;

    self->cap = va_arg(*ap, size_t);
    if (self->cap == 0)
        self->cap = SHMAP_DEFAULT_INITIAL_CAPACITY;

    self->heads = calloc(self->cap, sizeof(hlist_t *));
    if (self->heads == NULL)
        return ERR_PTR(-ENOMEM);

    self->seed = rand();
    self->hash = va_arg(*ap, hash_func_t);
    assert(self->hash != NULL);

    return self;
}

void shmap_dtor(void *_class) {
    shmap_t *self = _class;

    hlist_t **head;
    shmap_heads_for_each(self, head) {
        if (*head == NULL)
            continue;

        hlist_t *node, *n;
        hlist_for_each_safe(*head, node, n) {
            free(node);
        }
    }

    free(self->heads);
}

hash_t shmap_top_hash(const shmap_t *self, const mkey_t key) {
    return self->hash(self->seed, key) % self->cap;
}

int shmap_grow(shmap_t *self) {
    hlist_t **old_heads = self->heads;
    const size_t old_cap = self->cap;
    memcpy(old_heads, self->heads, self->cap * sizeof(hlist_t *));

    self->cap *= SHMAP_GROW_FACTOR;
    void *tmp = malloc(self->cap * sizeof(hlist_t *));
    if (tmp == NULL)
        return ENOMEM;
    free(self->heads);
    self->heads = tmp;

    for (size_t i = 0; i < old_cap; i++) {
        hlist_t *node, *n;
        hlist_for_each_safe(old_heads[i], node, n) {
            const hash_t top_hash = shmap_top_hash(self, node->key);
            node->next = self->heads[top_hash];
            self->heads[top_hash] = node;
        }
    }
    free(old_heads);
}

void shmap_insert(void *_self, const mkey_t key, const mval_t value) {
    shmap_t *self = _self;

    const hash_t top_hash = shmap_top_hash(self, key);
    self->heads[top_hash] = hlist_insert_head(self->heads[top_hash], key, value);

    while (hlist_count(self->heads[top_hash]) > SHMAP_HEIGHT_THRESHOLD_TO_GROW)
        shmap_grow(self);
}

map_res_t shmap_lookup(const void *_self, const mkey_t key) {
    const shmap_t *self = _self;

    hlist_t **head;
    shmap_heads_for_each(self, head) {
        const hlist_t *found = hlist_lookup(*head, key);
        if (found) {
            return (map_res_t){
                .data = found->value,
                .ok   = 1,
            };
        }
    };

    return (map_res_t){0};
}

int shmap_remove(void *_self, const mkey_t key) {
    shmap_t *self = _self;

    if (!shmap_lookup(self, key).ok)
        return 0;

    const hash_t top_hash = shmap_top_hash(self, key);
    self->heads[top_hash] = hlist_delete(self->heads[top_hash], key);

    return 1;
}

const imap_t SimpleHashMapClass = {
    .size   = sizeof(shmap_t),
    .ctor   = shmap_ctor,
    .dtor   = shmap_dtor,
    .insert = shmap_insert,
    .lookup = shmap_lookup,
    .remove = shmap_remove,
};
