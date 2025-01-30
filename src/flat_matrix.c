#include "flat_matrix.h"

#include <stdlib.h>

#include "err.h"


typedef struct {
    const imatrix_t *class; // Реализация интерфейса матрицы.
    val_t *data;
    int    rows;
    int    cols;
} flat_matrix_t;

_Static_assert(offsetof(flat_matrix_t, class) == 0);


void *flat_matrix_ctor(void *_class, va_list *ap) {
    flat_matrix_t *self = _class;

    self->rows = va_arg(*ap, int);
    self->cols = va_arg(*ap, int);
    if (self->rows < 1 || self->cols < 1)
        return ERR_PTR(-EINVAL);

    self->data = calloc(self->rows * self->cols, sizeof(val_t));
    if (self->data == NULL)
        return ERR_PTR(-ENOMEM);

    return self;
}

void flat_matrix_dtor(void *_self) {
    flat_matrix_t *self = _self;
    free(self->data);
    self->data = NULL;
}

int flat_matrix_rows(const void *_self) {
    const flat_matrix_t *self = _self;
    return self->rows;
}

int flat_matrix_cols(const void *_self) {
    const flat_matrix_t *self = _self;
    return self->cols;
}

void flat_matrix_set(void *_self, const int i, const int j, const val_t value) {
    const flat_matrix_t *self = _self;
    self->data[i * self->cols + j] = value;
}

val_t flat_matrix_get(const void *_self, const int i, const int j) {
    const flat_matrix_t *self = _self;
    return self->data[i * self->cols + j];
}

const imatrix_t FlatMatrixClass = {
    .size = sizeof(flat_matrix_t),
    .ctor = flat_matrix_ctor,
    .dtor = flat_matrix_dtor,
    .rows = flat_matrix_rows,
    .cols = flat_matrix_cols,
    .set  = flat_matrix_set,
    .get  = flat_matrix_get,
};
