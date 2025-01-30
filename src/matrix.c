#include "matrix.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "str.h"

void *matrix_new(const imatrix_t *class, ...) {
    void *p = malloc(class->size);
    if (p == NULL)
        return ERR_PTR(-ENOMEM);

    *(const imatrix_t **)p = class;
    if (class->ctor) {
        va_list ap;
        va_start(ap, class);
        p = class->ctor(p, &ap);
        va_end(ap);
    }

    return p;
}

void matrix_destroy(void *self) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);

    if ((*cp)->dtor)
        (*cp)->dtor(self);

    free(self);
}

int matrix_rows(const void *self) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->rows);
    return (*cp)->rows(self);
}

int matrix_cols(const void *self) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->cols);
    return (*cp)->cols(self);
}

void matrix_set(void *self, const int i, const int j, const val_t val) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->set);
    (*cp)->set(self, i, j, val);
}

val_t matrix_get(const void *self, const int i, const int j) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->get);
    return (*cp)->get(self, i, j);
}

void *matrix_scan_coord(const imatrix_t *class, FILE *in) {
    int m;
    if (fscanf(in, "%d", &m) != 1)
        return ERR_PTR(-EIO);
    if (m <= 0)
        return ERR_PTR(-EINVAL);

    int n;
    if (fscanf(in, "%d", &n) != 1)
        return ERR_PTR(-EIO);
    if (n <= 0)
        return ERR_PTR(-EINVAL);

    void *self = matrix_new(class, m, n);
    if (IS_ERR(self))
        return ERR_CAST(self);

    int c;
    while (c = fgetc(in), c != EOF && ungetc(c, in)) {
        int i, j;
        val_t val;
        if (fscanf(in, "%d %d" MATRIX_VAL_SCN, &i, &j, &val) != 3)
            break;
        matrix_set(self, i, j, val);
    }

    return self;
}

void matrix_fill(void *self, ...) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->set && (*cp)->rows && (*cp)->cols);

    va_list ap;
    va_start(ap, self);
    for (int i = 0; i < matrix_rows(self); i++) {
        for (int j = 0; j < matrix_cols(self); j++)
            (*cp)->set(self, i, j, va_arg(ap, val_t));
    }
    va_end(ap);
}

void matrix_print(const void *self, FILE *out) {
    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert((*cp)->get && (*cp)->rows && (*cp)->cols);

    // Если матрица имеет реализацию печати, используем её.
    if ((*cp)->print) {
        (*cp)->print(self, out);
        return;
    }

    // Если нет, то используем формат печати по умолчанию.
    for (int i = 0; i < matrix_rows(self); i++) {
        for (int j = 0; j < matrix_cols(self); j++)
            fprintf(out,
                MATRIX_VAL_PRI "%c",
                (*cp)->get(self, i, j),
                j == matrix_cols(self) - 1 ? '\n' : '\t'
            );
    }
}

void *matrix_sum(const void *self, const void *aug) {
    if (matrix_rows(self) != matrix_rows(aug) || matrix_cols(self) != matrix_cols(aug))
        return ERR_PTR(-EINVAL);

    const int m = matrix_rows(self);
    const int n = matrix_cols(self);

    const imatrix_t *const *cp = self;
    assert(self && *cp);
    assert(aug);

    // Совпадение указателей на дескрипторы есть совпадение классов.
    // Если классы совпадают, то мы можем использовать нативное сложение
    // матриц для увеличения производительности.
    // Нативное сложение матриц может быть не реализовано.
    if (*(imatrix_t *const *) self == *(imatrix_t *const *) aug && (*cp)->sum)
        return (*cp)->sum(self, aug);

    // Иначе используем классический алгоритм сложения матриц.

    void *sum = matrix_new(*cp, m, n);
    if (IS_ERR(sum))
        return ERR_CAST(sum);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            matrix_set(sum, i, j, matrix_get(self, i, j) + matrix_get(aug, i, j));
    }

    return sum;
}
