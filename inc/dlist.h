#ifndef DLIST_H
#define DLIST_H

#ifndef T
#error "T is not defined"
#endif // T

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "generic.h"

#define DNODE(type) GENERIC_TYPE(dnode, type)
#define DNODE_STRUCT(type) GENERIC_STRUCT(dnode, type)
// Узел двусвязанного циклического списка.
typedef DNODE_STRUCT(T) {
    DNODE_STRUCT(T) *next;
    DNODE_STRUCT(T) *prev;
    T                data;
} DNODE(T);

#define DLIST_CREATE(type) GENERIC_METHOD(dlist, create, type)
// Аллоцирует и инициализирует узел двусвязанного списка со значением value или
// возвращает ошибку выделения памяти ENOMEM (err.h).
static inline DNODE(T) *DLIST_CREATE(T) (const T data) {
    DNODE(T) *node = malloc(sizeof(DNODE(T)));
    if (node == NULL)
        return ERR_PTR(-ENOMEM);
    node->data = data;
    node->next = node;
    node->prev = node;
    return node;
};

#define DLIST_IS_FIRST(type) GENERIC_METHOD(dlist, is_first, type)
// Возвращает 1, если узел списка является первым после головы.
static inline int DLIST_IS_FIRST(T) (const DNODE(T) *head, const DNODE(T) *node) {
    return head->next == node;
}

#define DLIST_IS_LAST(type) GENERIC_METHOD(dlist, is_last, type)
// Возвращает 1, если узел списка является последним перед головой.
static inline int DLIST_IS_LAST(T) (const DNODE(T) *head, const DNODE(T) *node) {
    return head->prev == node;
}

#define DLIST_ADD(type) GENERIC_METHOD(dlist, add, type)
// Вставляет узел new между prev и next.
static inline void DLIST_ADD(T) (DNODE(T) *prev, DNODE(T) *new, DNODE(T) *next) {
    assert(prev);
    assert(new);
    assert(next);
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

#define DLIST_ADD_TAIL(type) GENERIC_METHOD(dlist, add_tail, type)
// Добавляет узел в конец. Возвращает голову списка.
static inline DNODE(T) *DLIST_ADD_TAIL(T) (DNODE(T) *head, DNODE(T) *tail) {
    if (head == NULL)
        return tail;
    DLIST_ADD(T) (head->prev, tail, head);
    return head;
}

#define DLIST_ADD_HEAD(type) GENERIC_METHOD(dlist, add_head, type)
// Добавляет узел перед списком. Возвращает новую голову списка.
static inline DNODE(T) *DLIST_ADD_HEAD(T) (DNODE(T) *prev, DNODE(T) *head) {
    if (head == NULL)
        return prev;
    DLIST_ADD(T) (head->prev, prev, head);
    return prev;
}

#define DLIST_ENTRY(type) GENERIC_METHOD(dlist, entry, type)
// Возвращает данные узла.
static inline T DLIST_ENTRY(T) (const DNODE(T) *node) {
    return node->data;
}

// Цикл с итератором по двусвязанному циклу.
// Изменение списка внутри цикла с итератором приводит к неопределённому
// поведению.
#define dlist_for_each(head, node)                                       \
    for ((node) = (head)->next; (node) != (head); (node) = (node)->next)

// Цикл с итератором назад по двусвязанному циклу.
// Изменение списка внутри цикла с итератором приводит к неопределённому
// поведению.
#define dlist_for_each_prev(head, node)                                  \
    for ((node) = (head)->prev; (node) != (head); (node) = (node)->prev)

// Безопасный к изменению списка цикл с итератором.
#define dlist_for_each_safe(head, node, n)          \
    for ((node) = (head)->next, (n) = (node)->next; \
         (node) != (head);                          \
         (node) = (n), (n) = (n)->next)

// Безопасный к изменению списка цикл с итератором назад.
#define dlist_for_each_prev_safe(head, node, n)     \
    for ((node) = (head)->prev, (n) = (node)->prev; \
         (node) != (head);                          \
         (node) = (n), (n) = (n)->prev)

#define DLIST_DESTROY(type) GENERIC_METHOD(dlist, destroy, type)
// Освобождение памяти из-под всех узлов списка.
static inline void DLIST_DESTROY(T) (DNODE(T) *head) {
    DNODE(T) *node, *n;
    dlist_for_each_safe(head, node, n)
        free(node);
}

#endif // DLIST_H
