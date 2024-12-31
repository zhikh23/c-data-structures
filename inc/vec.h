#ifndef VEC_H
#define VEC_H

#ifndef T
#error "T is not defined"
#endif

#include <stdlib.h>

#include "err.h"
#include "generic.h"

// Коэффициент, с которым вектор будет увеличивать свою ёмкость.
#define VEC_GROW_FACTOR 2

// Коэффициент, с которым вектор будет уменьшать свою ёмкость.
#define VEC_SHRINK_FACTOR 0.25

#define VEC(type) GENERIC_TYPE(vec, type)
// Саморасширяющийся динамический массив.
// Структура переменной длины.
typedef struct {
    // Количество элементов, которое может содержаться в векторе без
    // перевыделения памяти.
    size_t cap;

    // Количество элементов в векторе.
    size_t len;

    // Массив элементов, длиной cap.
    T data[];
} VEC(T);

#define VEC_CREATE(type) GENERIC_METHOD(vec, create, type)
// Аллоцирует вектор в памяти с заданной начальной ёмкостью cap.
// Возвращает указатель на вектор или ошибку выделения памяти ENOMEM (err.h).
static inline VEC(T) *VEC_CREATE(T) (const size_t cap) {
    VEC(T) *self = calloc(1, sizeof(VEC(T)) + cap * sizeof(T));
    if (self == NULL)
        return ERR_PTR(-ENOMEM);
    self->cap = cap;
    return self;
}

#define VEC_LEN(type) GENERIC_METHOD(vec, len, type)
// Возвращает количество элементов в векторе.
static inline size_t VEC_LEN(T) (const VEC(T) *self) {
    return self->len;
}

#define VEC_IS_EMPTY(type) GENERIC_METHOD(vec, is_empty, type)
// Возвращает 1, если вектор пустой; иначе 0.
static inline size_t VEC_IS_EMPTY(T) (const VEC(T) *self) {
    return self->len == 0;
}

#define VEC_ENTRY(type) GENERIC_METHOD(vec, entry, type)
// Возвращает запись по заданному индексу.
// Если i >= self->len, поведение не определено.
static inline T VEC_ENTRY(T) (const VEC(T) *self, const size_t i) {
    return self->data[i];
}

#define VEC_GROW(type) GENERIC_METHOD(vec, grow, type)
// Увеличивает ёмкость вектора. Возвращает реаллоцированный вектор
// или ошибку выделения памяти ENOMEM (err.h).
static inline VEC(T) *VEC_GROW(T) (VEC(T) *self) {
    const size_t new_cap = self->cap == 0 ? 1 : self->cap * VEC_GROW_FACTOR;
    VEC(T) *new = realloc(self, sizeof(VEC(T)) + new_cap * sizeof(T));
    if (new == NULL)
        return ERR_PTR(-ENOMEM);
    self = new;
    self->cap = new_cap;
    return self;
}

#define VEC_PUSH(type) GENERIC_METHOD(vec, push, type)
// Добавляет элемент value в конец вектора.
// Так как вектор может увеличить свой размер, возвращает
// указатель на тот же вектор или реаллоцированный вектор
// с большей ёмкостью.
static inline VEC(T) *VEC_PUSH(T) (VEC(T) *self, const T value) {
    if (self->len >= self->cap) {
        VEC(T) *new = VEC_GROW(T)(self);
        if (IS_ERR(new))
            return ERR_CAST(new);
        self = new;
    }

    self->data[self->len] = value;
    self->len++;

    return self;
}

#define VEC_SHRINK(type) GENERIC_METHOD(vec, shrink, type)
// Уменьшает ёмкость вектора. Возвращает реаллоцированный вектор
// или ошибку выделения памяти ENOMEM (err.h).
static inline VEC(T) *VEC_SHRINK(T) (VEC(T) *self) {
    const size_t new_cap = self->cap * VEC_SHRINK_FACTOR;
    VEC(T) *new = realloc(self, sizeof(VEC(T)) + new_cap * sizeof(T));
    if (new == NULL)
        return NULL;
    self = new;
    self->cap = new_cap;
    return self;
}

#define VEC_POP(type) GENERIC_METHOD(vec, pop, type)
// Удаляет последний элемент вектора.
// Так как вектор может уменьшить свой размер, возвращает
// указатель на тот же вектор или реаллоцированный вектор
// с меньшей ёмкостью.
static inline VEC(T) *VEC_POP(T) (VEC(T) *self) {
    if ((double)self->cap * VEC_SHRINK_FACTOR >= (double)self->len) {
        VEC(T) *new = VEC_SHRINK(T)(self);
        if (new == NULL)
            return ERR_PTR(-ENOMEM);
        self = new;
    }
    self->len--;
    return self;
}

// Цикл с итератором iter по вектору self.
// Изменение вектора внутри цикла с итератором приводит к неопределённому
// поведению.
#define vec_for_each(self, iter) \
    for ((iter) = (self)->data; (iter) < (self)->data + (self)->len; (iter)++)

#endif // VEC_H
