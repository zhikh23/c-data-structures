#include "bstree.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"

typedef struct {
    mut_mkey_t key;
    mval_t     value;
} pair_t;

struct bstree_node;
typedef struct bstree_node bstree_node_t;

struct bstree_node {
    pair_t         data;
    bstree_node_t *left;
    bstree_node_t *right;
};

typedef struct {
    const imap_t *class;
    bstree_node_t *root;
} bstree_t;

_Static_assert(offsetof(bstree_t, class) == 0);

void *bstree_ctor(void *_class, va_list *ap) {
    bstree_t *bst = _class;
    bst->root = NULL;
    return bst;
}

void bstree_node_destroy(bstree_node_t *node) {
    if (node != NULL) {
        bstree_node_destroy(node->left);
        bstree_node_destroy(node->right);
        free(node->data.key);
        free(node);
    }
}

void bstree_destroy(void *_self) {
    bstree_t *self = _self;
    bstree_node_destroy(self->root);
    self->root = NULL;
}

bstree_node_t *bstree_node_create(const mkey_t key, const mval_t value) {
    bstree_node_t *node = malloc(sizeof(bstree_node_t));
    if (node == NULL)
        return ERR_PTR(-ENOMEM);

    node->left = NULL;
    node->right = NULL;
    node->data = (pair_t){ strdup(key), value };

    return node;
}

static bstree_node_t *bstree_node_insert(bstree_node_t *self, const mkey_t key, const mval_t value) {
    assert(key);

    // If root does not exist, new node will be root of bstree.
    if (self == NULL) {
        bstree_node_t *node = bstree_node_create(key, value);
        if (IS_ERR(node))
            return ERR_CAST(node);
        return node;
    }

    const int cmp = strcmp(key, self->data.key);
    if (cmp < 0)
        self->left = bstree_node_insert(self->left, key, value);
    else if (cmp > 0)
        self->right = bstree_node_insert(self->right, key, value);
    else
        self->data.value = value;

    return self;
}

void bstree_insert(void *_self, const mkey_t key, const mval_t value) {
    bstree_t *self = _self;
    self->root = bstree_node_insert(self->root, key, value);
}

static bstree_node_t *bstree_node_lookup(bstree_node_t *self, const char *key) {
    assert(key);

    if (self == NULL)
        return NULL;

    const int cmp = strcmp(key, self->data.key);
    if (cmp < 0)
        return bstree_node_lookup(self->left, key);
    if (cmp > 0)
        return bstree_node_lookup(self->right, key);

    return self;
}

static bstree_node_t *bstree_min_node(bstree_node_t *self) {
    if (self == NULL)
        return NULL;

    while (self->left != NULL)
        self = self->left;

    return self;
}

map_res_t bstree_lookup(const void *_self, const mkey_t key) {
    const bstree_t *self = _self;

    const bstree_node_t *found = bstree_node_lookup(self->root, key);
    if (found == NULL)
        return (map_res_t){0};

    return (map_res_t){
        .data = found->data.value,
        .ok   = 1,
    };
}

static bstree_node_t *bstree_node_remove(bstree_node_t *self, const char *key) {
    if (self == NULL)
        return NULL;

    const int cmp = strcmp(key, self->data.key);
    if (cmp < 0) {
        self->left = bstree_node_remove(self->left, key);
        return self;
    }
    if (cmp > 0) {
        self->right = bstree_node_remove(self->right, key);
        return self;
    }

    // cmp == 0

    if (self->left == NULL && self->right == NULL) {
        free(self->data.key);
        free(self);
        return NULL;
    }

    // One branch: right
    //  1             3
    //   \           / \
    //    3    ->   2   4
    //   / \
    //  2   4

    if (self->left == NULL) {
        bstree_node_t *next = self->right;
        free(self->data.key);
        free(self);
        return next;
    }

    // One branch: left
    //      4         2
    //     /         / \
    //    2    ->   1   3
    //   / \
    //  1   3

    if (self->right == NULL) {
        bstree_node_t *next = self->left;
        free(self->data.key);
        free(self);
        return next;
    }

    // Both branches
    //      3              4             4
    //     / \            / \           / \
    //    2   5    ->    2   5    ->   2   5
    //   /   / \        /   / \       /     \
    //  1   4   6      1   4   6     1       6
    //      ^
    //  min in the
    //  right branch

    const bstree_node_t *min = bstree_min_node(self->right);
    self->data = min->data;
    self->right = bstree_node_remove(min->right, min->data.key);
    return self;
}

int bstree_remove(void *_self, const mkey_t key) {
    bstree_t *self = _self;

    if (!bstree_lookup(_self, key).ok)
        return 0;

    self->root = bstree_node_remove(self->root, key);

    return 1;
}

const imap_t BSTreeClass = {
    .size   = sizeof(bstree_t),
    .ctor   = bstree_ctor,
    .dtor   = bstree_destroy,
    .insert = bstree_insert,
    .lookup = bstree_lookup,
    .remove = bstree_remove,
};
