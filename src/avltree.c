#include "avltree.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"

typedef struct {
    mut_mkey_t key;
    mval_t     value;
} pair_t;

struct avltree_node;
typedef struct avltree_node avltree_node_t;

struct avltree_node {
    pair_t          data;
    avltree_node_t *left;
    avltree_node_t *right;
    int             height;
};

typedef struct {
    const imap_t   *class;
    avltree_node_t *root;
} avltree_t;

_Static_assert(offsetof(avltree_t, class) == 0);

void *avltree_ctor(void *_class, va_list *ap) {
    avltree_t *bst = _class;
    bst->root = NULL;
    return bst;
}

void avltree_node_destroy(avltree_node_t *node) {
    if (node != NULL) {
        avltree_node_destroy(node->left);
        avltree_node_destroy(node->right);
        free(node->data.key);
        free(node);
    }
}

void avltree_destroy(void *_self) {
    avltree_t *self = _self;
    avltree_node_destroy(self->root);
    self->root = NULL;
}

avltree_node_t *avltree_node_create(const mkey_t key, const mval_t value) {
    avltree_node_t *node = malloc(sizeof(avltree_node_t));
    if (node == NULL)
        return ERR_PTR(-ENOMEM);

    node->left = NULL;
    node->right = NULL;
    node->data = (pair_t){ strdup(key), value };
    node->height = 1;

    return node;
}

static inline int avltree_node_height(const avltree_node_t *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

static inline int avltree_node_balance(const avltree_node_t *node) {
    if (node == NULL)
        return 0;
    return avltree_node_height(node->left) - avltree_node_height(node->right);
}

static inline int max(const int x, const int y) {
    return x > y ? x : y;
}

static avltree_node_t *avltree_right_rotate(avltree_node_t *y) {
    avltree_node_t *x = y->left;
    avltree_node_t *t2 = x->right;
    x->right = y;
    y->left = t2;
    y->height = 1 + max(avltree_node_height(y->left), avltree_node_height(y->right));
    x->height = 1 + max(avltree_node_height(x->left), avltree_node_height(x->right));
    return x;
}

static avltree_node_t *avltree_left_rotate(avltree_node_t *x) {
    avltree_node_t *y = x->right;
    avltree_node_t *t2 = y->left;
    y->left = x;
    x->right = t2;
    x->height = 1 + max(avltree_node_height(x->left), avltree_node_height(x->right));
    y->height = 1 + max(avltree_node_height(y->left), avltree_node_height(y->right));
    return y;
}

static avltree_node_t *avltree_balance(avltree_node_t *node) {
    const int balance = avltree_node_balance(node);

    // Случай LL.
    //       3          2
    //      /          / \
    //     2     ->   1   3
    //    /
    //   1
    if (balance > 1 && avltree_node_balance(node->left) >= 0)
        return avltree_right_rotate(node);

    // Случай LR.
    //    4          4        3
    //   /          /        / \
    //  2     ->   3    ->  2   4
    //   \        /
    //    3      2
    if (balance > 1 && avltree_node_balance(node->left) < 0) {
        node->left = avltree_left_rotate(node->left);
        return avltree_right_rotate(node);
    }

    // Случай RR.
    //   1            2
    //    \          / \
    //     2    ->  1   3
    //      \
    //       3
    if (balance < -1 && avltree_node_balance(node->right) <= 0)
        return avltree_left_rotate(node);

    // Случай RL.
    //  2        2            3
    //   \        \          / \
    //    4   ->   3    ->  2   4
    //   /          \
    //  3            4
    if (balance < -1 && avltree_node_balance(node->right) > 0) {
        node->right = avltree_right_rotate(node->right);
        return avltree_left_rotate(node);
    }

    return node;
}

static avltree_node_t *avltree_node_insert(avltree_node_t *node, const mkey_t key, const mval_t value) {
    assert(key);

    // Если корня нет, то новый узел становится корнем.
    if (node == NULL) {
        avltree_node_t *new_root = avltree_node_create(key, value);
        if (IS_ERR(new_root))
            return ERR_CAST(new_root);
        return new_root;
    }

    const int cmp = strcmp(key, node->data.key);
    if (cmp < 0)
        node->left = avltree_node_insert(node->left, key, value);
    else if (cmp > 0)
        node->right = avltree_node_insert(node->right, key, value);
    else
        node->data.value = value; // обновляем существующее значение

    node->height = 1 + max(avltree_node_height(node->left), avltree_node_height(node->right));
    return avltree_balance(node);
}

void avltree_insert(void *_self, const mkey_t key, const mval_t value) {
    avltree_t *self = _self;
    self->root = avltree_node_insert(self->root, key, value);
}

static avltree_node_t *avltree_node_lookup(avltree_node_t *self, const char *key) {
    assert(key);

    if (self == NULL)
        return NULL;

    const int cmp = strcmp(key, self->data.key);
    if (cmp < 0)
        return avltree_node_lookup(self->left, key);
    if (cmp > 0)
        return avltree_node_lookup(self->right, key);

    return self;
}

static avltree_node_t *avltree_min_node(avltree_node_t *self) {
    if (self == NULL)
        return NULL;

    while (self->left != NULL)
        self = self->left;

    return self;
}

map_res_t avltree_lookup(const void *_self, const mkey_t key) {
    const avltree_t *self = _self;

    const avltree_node_t *found = avltree_node_lookup(self->root, key);
    if (found == NULL)
        return (map_res_t){0};

    return (map_res_t){
        .data = found->data.value,
        .ok   = 1,
    };
}

static avltree_node_t *avltree_node_remove(avltree_node_t *node, const char *key) {
    if (node == NULL)
        return NULL;

    const int cmp = strcmp(key, node->data.key);
    if (cmp < 0) {
        node->left = avltree_node_remove(node->left, key);
    } else if (cmp > 0) {
        node->right = avltree_node_remove(node->right, key);
    } else {
        // cmp == 0

        // Нет ветвей, лист.
        if (node->left == NULL && node->right == NULL) {
            free(node->data.key);
            free(node);
            return NULL;
        }

        // Одна ветвь: правая.
        //  1             3
        //   \           / \
        //    3    ->   2   4
        //   / \
        //  2   4
        if (node->left == NULL) {
            avltree_node_t *next = node->right;
            free(node->data.key);
            free(node);
            return next;
        }

        // Одна ветвь: левая.
        //      4         2
        //     /         / \
        //    2    ->   1   3
        //   / \
        //  1   3
        if (node->right == NULL) {
            avltree_node_t *next = node->left;
            free(node->data.key);
            free(node);
            return next;
        }

        // У узла есть обе ветви. Выбирается минимальный элемент
        // в правой ветви или максимальный в левой и встаёт на место
        // корня.

        //      3              4             4
        //     / \            / \           / \
        //    2   5    ->    2   5    ->   2   5
        //   /   / \        /   / \       /     \
        //  1   4   6      1   4   6     1       6
        //      ^
        //  минимальный в
        //  правой  ветви
        const avltree_node_t *min = avltree_min_node(node->right);
        free(node->data.key);
        node->data.key = strdup(min->data.key);
        node->data.value = min->data.value;
        node->right = avltree_node_remove(node->right, min->data.key);
    }

    node->height = max(avltree_node_height(node->left), avltree_node_height(node->right));
    return avltree_balance(node);
}

int avltree_remove(void *_self, const mkey_t key) {
    avltree_t *self = _self;

    if (avltree_lookup(_self, key).ok == 0)
        return 0;

    self->root = avltree_node_remove(self->root, key);

    return 1;
}

const imap_t AVLTreeClass = {
    .size   = sizeof(avltree_t),
    .ctor   = avltree_ctor,
    .dtor   = avltree_destroy,
    .insert = avltree_insert,
    .lookup = avltree_lookup,
    .remove = avltree_remove,
};
