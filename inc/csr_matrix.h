/**
 * csr_matrix.h - реализация разреженной CSR матрицы (matrix.h)
 *                с построчным хранением.
 */
#ifndef CSR_MATRIX_H
#define CSR_MATRIX_H

#include "matrix.h"

extern const imatrix_t CSRMatrixClass;
// matrix_new(CSRMatrix, rows, cols)
static const imatrix_t *CSRMatrix = &CSRMatrixClass;

#endif // CSR_MATRIX_H
