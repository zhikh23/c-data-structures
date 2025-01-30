/**
 * flat_matrix.h - простейшая реализация матрицы (matrix.h)
 *                 на одном динамическом массиве.
 *
 * Элементы матрицы хранятся единой областью памяти построчно.
 */
#ifndef FLAT_MATRIX_H
#define FLAT_MATRIX_H

#include "matrix.h"

extern const imatrix_t FlatMatrixClass;
// matrix_new(FlatMatrix, rows, cols)
static const imatrix_t *FlatMatrix = &FlatMatrixClass;

#endif // FLAT_MATRIX_H
