#include "csr_matrix.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"


#define CSR_MATRIX_INITIAL_CAP 4
#define CSR_MATRIX_GROW_FACTOR 2

#define CSR_MATRIX_COORD_PRINT_THRESHOLD 10000

#define CSR_MATRIX_IS_FULL(self) ((self)->nnz >= (self)->cap)


typedef struct {
    const imatrix_t *class; // Реализация интерфейса матрицы.

    // Количество строк в матрице.
    int rows;

    // Количество столбцов в матрице.
    int cols;

    // Ненулевые элементы матрицы.
    val_t *data;

    // Индексы столбцов ненулевых элементов в матрице.
    int *col_idx;

    // Указатели на начала строк.
    // Длина массива постоянна и равна количеству строк + 1.
    int *row_ptr;

    // Количество ненулевых элементов в матрице или длина
    // массивов data и col_idx соответственно.
    int nnz;

    // Количество ненулевых элементов, которые можно добавить в матрицу
    // без перевыделения памяти (роста).
    int cap;
} csr_matrix_t;

_Static_assert(offsetof(csr_matrix_t, class) == 0);


void *csr_matrix_ctor(void *_class, va_list *ap) {
    csr_matrix_t *self = _class;

    self->rows = va_arg(*ap, int);
    self->cols = va_arg(*ap, int);
    if (self->rows < 1 || self->cols < 1)
        return ERR_PTR(-EINVAL);

    // По умолчанию матрица создаётся пустой.
    self->data = NULL;
    self->col_idx = NULL;
    self->nnz = 0;
    self->cap = 0;

    // Известно количество строк, можно сразу выделить всю необходимую
    // память под все строки.
    self->row_ptr = calloc(self->rows + 1, sizeof(int));

    return self;
}

void csr_matrix_dtor(void *_self) {
    csr_matrix_t *self = _self;
    free(self->data);
    self->data = NULL;
    free(self->col_idx);
    self->col_idx = NULL;
    free(self->row_ptr);
    self->row_ptr = NULL;
}

void *csr_matrix_copy(const void *_self) {
    const csr_matrix_t *self = _self;
    csr_matrix_t *new = malloc(sizeof(csr_matrix_t));
    if (new == NULL)
        return ERR_PTR(-ENOMEM);

    new->class = self->class;
    new->rows  = self->rows;
    new->cols  = self->cols;
    new->nnz   = self->nnz;
    new->cap   = self->cap;

    if (new->cap > 0) {
        new->data = malloc(new->cap * sizeof(val_t));
        if (self->data == NULL) {
            free(new);
            return ERR_PTR(-ENOMEM);
        }
        memcpy(new->data, self->data, self->cap * sizeof(val_t));
    } else
        new->data = NULL;


    if (new->cap > 0) {
        new->col_idx = malloc(new->cap * sizeof(int));
        if (self->col_idx == NULL) {
            free(new->data);
            free(new);
            return ERR_PTR(-ENOMEM);
        }
        memcpy(new->col_idx, self->col_idx, self->cap * sizeof(int));
    } else
        new->col_idx = NULL;

    // Известно количество строк, можно сразу выделить всю необходимую
    // память под все строки.
    new->row_ptr = malloc((self->rows + 1) * sizeof(int));
    if (self->row_ptr == NULL) {
        free(new->col_idx);
        free(new->data);
        free(new);
        return ERR_PTR(-ENOMEM);
    }
    memcpy(new->row_ptr, self->row_ptr, (self->rows + 1) * sizeof(int));

    return new;
}

int csr_matrix_rows(const void *_self) {
    const csr_matrix_t *self = _self;
    return self->rows;
}

int csr_matrix_cols(const void *_self) {
    const csr_matrix_t *self = _self;
    return self->cols;
}

int csr_matrix_grow(csr_matrix_t *self) {
    const int new_cap = (self->cap) ? self->cap * CSR_MATRIX_GROW_FACTOR : CSR_MATRIX_INITIAL_CAP;

    val_t *new_data = realloc(self->data, sizeof(val_t) * new_cap);
    if (!new_data)
        return ENOMEM;

    int *new_col_idx = realloc(self->col_idx, sizeof(int) * new_cap);
    if (!new_col_idx) {
        free(self->data);
        return ENOMEM;
    }

    self->data = new_data;
    self->col_idx = new_col_idx;
    self->cap = new_cap;

    return 0;
}

void csr_matrix_set(void *_self, const int i, const int j, const val_t val) {
    csr_matrix_t *self = _self;
    assert(i >= 0);
    assert(j >= 0);
    assert(i < self->rows);
    assert(j < self->cols);

    // Проверяем, присутствует ли данный элемент в матрице.
    // Если да, то обновляем его.
    for (int k = self->row_ptr[i]; k < self->row_ptr[i + 1]; k++) {
        if (self->col_idx[k] == j) {
            self->data[k] = val;
            return;
        }
    }

    // В предыдущем шаге set(0.0) имел смысл, так как можно было
    // обнулить элемент в матрице.
    // Далее мы гарантируем, что ij-ый элемент не существует в массиве
    // ненулевых элементов, то есть ij-ый элемент есть нуль.
    // Значит установка данного элемента в нуль не имеет смысла.
    if (val == 0.0)
        return;

    // Если матрица уже имеет максимальное количество ненулевых элементов,
    // расширяем её.
    if (CSR_MATRIX_IS_FULL(self)) {
        const int err = csr_matrix_grow(self);
        assert(err == 0);   //  ужасно, но иначе ошибку вернуть не можем...
    }

    self->nnz++;

    // Находим позицию для вставки нового элемента
    int pos = self->row_ptr[i + 1] - 1;
    while (pos >= self->row_ptr[i] && self->col_idx[pos] > j)
        pos--;
    pos++; // следующий

    // Сдвигаем элементы вправо для вставки нового значения
    memmove(self->col_idx + pos + 1, self->col_idx + pos, self->nnz - pos - 1);
    memmove(self->data    + pos + 1, self->data    + pos, self->nnz - pos - 1);

    // Вставляем новое значение и индекс столбца
    self->data[pos] = val;
    self->col_idx[pos] = j;

    // Обновляем указатели на строки, следующие после
    // вставленного элемента.
    for (int k = i + 1; k <= self->rows; k++)
        self->row_ptr[k]++;
}

val_t csr_matrix_get(const void *_self, const int i, const int j) {
    const csr_matrix_t *self = _self;
    assert(i >= 0);
    assert(j >= 0);
    assert(i < self->rows);
    assert(j < self->cols);

    for (int k = self->row_ptr[i]; k < self->row_ptr[i + 1]; k++) {
        if (self->col_idx[k] == j)
            return self->data[k];
    }

    return 0.0; // Индекс элемента валидный, но его нет в массиве => он равен нулю
}

void csr_matrix_print_coord(const void *_self, FILE *out) {
    const csr_matrix_t *self = _self;

    for (int i = 0; i < self->rows; i++) {
        for (int k = self->row_ptr[i]; k < self->row_ptr[i + 1]; k++) {
            const int j = self->col_idx[k];
            fprintf(out, "%d\t%d\t" MATRIX_VAL_PRI "\n", i, j, self->data[k]);
        }
    }
}

void csr_matrix_print_common(const void *_self, FILE *out) {
    const csr_matrix_t *self = _self;

    for (int i = 0; i < matrix_rows(self); i++) {
        for (int j = 0; j < matrix_cols(self); j++)
            fprintf(out,
                MATRIX_VAL_PRI "%c",
                matrix_get(self, i, j),
                j == matrix_cols(self) - 1 ? '\n' : '\t'
            );
    }
}

void csr_matrix_print(const void *_self, FILE *out) {
    const csr_matrix_t *self = _self;

    // if (self->rows * self->cols >= CSR_MATRIX_COORD_PRINT_THRESHOLD)
        csr_matrix_print_coord(_self, out);
    // else
    //    csr_matrix_print_common(_self, out);
}

void *csr_matrix_sum(const void *_self, const void *_aug) {
    const csr_matrix_t *self = _self;
    const csr_matrix_t *aug  = _aug;

    // Копируем матрицу из левого слагаемого.
    // С высокой долей вероятности, в результирующей матрице ненулевые
    // элементы левого (правого) слагаемого останутся ненулевыми,
    // поэтому логично будет единым копированием перенести в результирующую
    // матрицу. Обновление существующего ненулевого значения значительно
    // более дешёвая операция, чем заполнение с нуля.
    // В идеале, выбирать для копирования ту матрицу, в которой больше всего
    // ненулевых элементов.
    csr_matrix_t *sum = csr_matrix_copy(self);
    if (IS_ERR(sum))
        return ERR_CAST(sum);

    // Проходим по всем ненулевым элементам добавляемой матрицы.
    for (int i = 0; i < aug->rows; i++) {
        for (int k = aug->row_ptr[i]; k < aug->row_ptr[i + 1]; k++) {
            const int j = aug->col_idx[k];
            matrix_set(sum, i, j, matrix_get(sum, i, j) + aug->data[k]);
        }
    }

    return sum;
}

const imatrix_t CSRMatrixClass = {
    .size  = sizeof(csr_matrix_t),
    .ctor  = csr_matrix_ctor,
    .dtor  = csr_matrix_dtor,
    .rows  = csr_matrix_rows,
    .cols  = csr_matrix_cols,
    .set   = csr_matrix_set,
    .get   = csr_matrix_get,
    .print = csr_matrix_print,
    .sum   = csr_matrix_sum,
};
