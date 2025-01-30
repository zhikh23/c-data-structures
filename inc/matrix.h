#ifndef MATRIX_H
#define MATRIX_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>


#define MATRIX_VAL_PRI "%.2lf"
#define MATRIX_VAL_SCN "%lf"


// Синоним типа для элемента матрицы.
typedef double val_t;

// Дескриптор (класс) матрицы.
typedef struct {
    // size указывает на объём памяти, требуемый для выделения
    // объекту класса перед вызовом конструктора.
    // Обычно:
    //     .size = sizeof(MatrixImplementation);
    size_t size;

    // Конструктор класса.
    void *(*ctor)(void *, va_list *);

    // Деструктор класса.
    void (*dtor)(void *);

    // Методы мапы.

    int (*rows)(const void *);
    int (*cols)(const void *);
    void (*set)(void *, int i, int j, val_t val);
    val_t (*get)(const void *, int i, int j);

    // Методы, доступные для override

    void (*print)(const void *, FILE *);
    // Метод для нативного сложения матриц двух типов.
    void *(*sum)(const void *, const void *);
} imatrix_t;

/**
 * Создаёт объект указанного класса.
 * Сначала выделяет память для объекта, затем вызывает конструктор класса.
 * @param  class класс, реализующий интерфейс imatrix_t.
 * @param  ...   параметры конструктора класса.
 * @return Проинициализированный объект класса или ошибку (err.h).
 */
void *matrix_new(const imatrix_t *class, ...);

/**
 * Освобождает память, использованную для объекта.
 * Сначала вызывает деструктор класса, если таковой имеется, затем
 * освобождает аллоцированную память в matrix_new.
 * @param self объект класса, реализующего интерфейс imatrix_t.
 */
void matrix_destroy(void *self);

/**
 * Возвращает количество строк в матрице.
 * @param  self объект класса, реализующего интерфейс imatrix_t.
 * @return Количество строк в матрице >0.
 */
int matrix_rows(const void *self);

/**
 * Возвращает количество строк в матрице.
 * @param  self объект класса, реализующего интерфейс imatrix_t.
 * @return Количество строк в матрице >0.
 */
int matrix_cols(const void *self);

/**
 * Устанавливает значение ij-ому элементу матрицы.
 * @param self объект класса, реализующего интерфейс imatrix_t.
 * @param i    номер строки матрицы, 0 <= i < rows.
 * @param j    номер столбца матрицы, 0 <= j < cols.
 * @param val  значение, устанавливаемое в матрицу.
 */
void matrix_set(void *self, int i, int j, val_t val);

/**
 * Возвращает значение ij-ого элемента матрицы.
 * @param self объект класса, реализующего интерфейс imatrix_t.
 * @param i    номер строки матрицы, 0 <= i < rows.
 * @param j    номер столбца матрицы, 0 <= j < cols.
 */
val_t matrix_get(const void *self, int i, int j);

/**
 * Считывает матрицу из файла, записанной в координатном виде.
 * @param  class класс, реализующий интерфейс imatrix_t.
 * @param  in    файл открытый на чтение.
 * @return Прочитанная матрица из файла или ошибка (err.h)
 */
void *matrix_scan_coord(const imatrix_t *class, FILE *in);

/**
 * Заполняет матрицу элементами.
 * @param self объект класса, реализующего интерфейс imatrix_t.
 * @param ...  элементы матрицы в порядке обхода по строкам.
 *
 * @attention Если количество элементов не совпадает с количеством
 *            элементов матрицы (rows x cols), поведение не определено.
 * @attention Необходимо обратить внимание на тип элемента матрицы, так как
 *            передача int-ов вместо double-ов приведёт к UB.
 */
void matrix_fill(void *self, ...);

/**
 * Печатает матрицу в файл в инженерном виде, используя в
 * качестве разделителя символ табуляции '\\t'.
 * @param self объект класса, реализующего интерфейс imatrix_t.
 * @param out  файл, открытый на запись.
 */
void matrix_print(const void *self, FILE *out);

/**
 * Складывает две матрицы равных размеров.
 * @param  self объект класса, реализующего интерфейс imatrix_t.
 * @param  aug  объект класса, реализующего интерфейс imatrix_t.
 * @return Матрица размером N x M класса левого слагаемого self.
 *         Если размеры матриц не совпадают, возвращает EINVAL (err.h).
 *         Если произошла ошибка выделения памяти, возвращает ENOMEM (err.h).
 */
void *matrix_sum(const void *self, const void *aug);

#endif // MATRIX_H
