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

static bstree_node_t *bstree_node_insert(bstree_node_t *node, const mkey_t key, const mval_t value) {
    assert(key);

    // Если корня нет, то новый узел становится корнем.
    if (node == NULL) {
        bstree_node_t *new_root = bstree_node_create(key, value);
        if (IS_ERR(new_root))
            return ERR_CAST(new_root);
        return new_root;
    }

    const int cmp = strcmp(key, node->data.key);
    if (cmp < 0)
        node->left = bstree_node_insert(node->left, key, value);
    else if (cmp > 0)
        node->right = bstree_node_insert(node->right, key, value);
    else
        node->data.value = value;

    return node;
}

void bstree_insert(void *_self, const mkey_t key, const mval_t value) {
    bstree_t *self = _self;
    self->root = bstree_node_insert(self->root, key, value);
}

static bstree_node_t *bstree_node_lookup(bstree_node_t *node, const char *key) {
    assert(key);

    if (node == NULL)
        return NULL;

    const int cmp = strcmp(key, node->data.key);
    if (cmp < 0)
        return bstree_node_lookup(node->left, key);
    if (cmp > 0)
        return bstree_node_lookup(node->right, key);

    return node;
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

static bstree_node_t *bstree_node_remove(bstree_node_t *node, const char *key) {
    if (node == NULL)
        return NULL;

    const int cmp = strcmp(key, node->data.key);
    if (cmp < 0) {
        node->left = bstree_node_remove(node->left, key);
        return node;
    }
    if (cmp > 0) {
        node->right = bstree_node_remove(node->right, key);
        return node;
    }

    // cmp == 0

    if (node->left == NULL && node->right == NULL) {
        free(node->data.key);
        free(node);
        return NULL;
    }

    // One branch: right
    //  1             3
    //   \           / \
    //    3    ->   2   4
    //   / \
    //  2   4

    if (node->left == NULL) {
        bstree_node_t *next = node->right;
        free(node->data.key);
        free(node);
        return next;
    }

    // One branch: left
    //      4         2
    //     /         / \
    //    2    ->   1   3
    //   / \
    //  1   3

    if (node->right == NULL) {
        bstree_node_t *next = node->left;
        free(node->data.key);
        free(node);
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

    const bstree_node_t *min = bstree_min_node(node->right);
    free(node->data.key);
    node->data.key = strdup(min->data.key);
    node->data.value = min->data.value;
    node->right = bstree_node_remove(node->right, min->data.key);
    return node;
}

int bstree_remove(void *_self, const mkey_t key) {
    bstree_t *self = _self;

    if (bstree_lookup(_self, key).ok == 0)
        return 0;

    self->root = bstree_node_remove(self->root, key);

    return 1;
}

const imap_t BinarySearchTreeClass = {
    .size   = sizeof(bstree_t),
    .ctor   = bstree_ctor,
    .dtor   = bstree_destroy,
    .insert = bstree_insert,
    .lookup = bstree_lookup,
    .remove = bstree_remove,
};
