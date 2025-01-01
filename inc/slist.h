#ifndef SLIST_H
#define SLIST_H

#ifndef T
#error "T is not defined"
#endif

#include <stdlib.h>

#include "err.h"
#include "generic.h"

#define SNODE(type) GENERIC_TYPE(snode, type)
#define SNODE_STRUCT(type) GENERIC_STRUCT(snode, type)
// Узел односвязанного списка.
typedef SNODE_STRUCT(T) {
    SNODE_STRUCT(T) *next;
    T                data;
} SNODE(T);

#define SLIST_CREATE(type) GENERIC_METHOD(slist, create, type)
// Аллоцирует и инициализирует узел односвязанного списка со значением value или
// возвращает ошибку выделения памяти ENOMEM (err.h).
static inline SNODE(T) *SLIST_CREATE(T) (const T value) {
    SNODE(T) *node = malloc(sizeof(SNODE(T)));
    if (node == NULL)
        return ERR_PTR(-ENOMEM);
    node->data = value;
    node->next = NULL;
    return node;
};

#define SLIST_IS_LAST(type) GENERIC_METHOD(slist, is_last, type)
// Возвращает 1, если node является последним элементом списка.
static inline int SLIST_IS_LAST(T) (const SNODE(T) *node) {
    return node->next == NULL;
}

#define SLIST_ENTRY(type) GENERIC_METHOD(slist, entry, type)
// Возвращает данные узла.
static inline T SLIST_ENTRY(T) (const SNODE(T) *node) {
    return node->data;
}

// Цикл с итератором node списка head.
// Изменение списка внутри цикла с итератором приводит к неопределённому
// поведению.
#define slist_for_each(head, node)                       \
    for ((node) = (head); (node); (node) = (node)->next)

// Безопасный к изменению списка цикл с итератором.
#define slist_for_each_safe(head, node, n)           \
    for ((node) = (head), (n) = (node)->next;        \
         (node);                                     \
         (node) = (n), (n) = (n) ? (n)->next : NULL)

#define SLIST_DESTROY(type) GENERIC_METHOD(slist, destroy, type)
// Освобождение памяти из-под всех узлов списка.
static inline void SLIST_DESTROY(T) (SNODE(T) *head) {
    SNODE(T) *node, *n;
    slist_for_each_safe(head, node, n)
        free(node);
}

#define SLIST_CONCAT(type) GENERIC_METHOD(slist, concat, type)
// Объединяет списки, добавляя к self в конец tail, возвращает голову self.
static inline SNODE(T) *SLIST_CONCAT(T) (SNODE(T) *self, SNODE(T) *tail) {
    if (self == NULL)
        return tail;
    SNODE(T) *node;
    for (node = self; !SLIST_IS_LAST(T) (node); node = node->next)
        ;
    node->next = tail;
    return self;
}

#endif // SLIST_H
