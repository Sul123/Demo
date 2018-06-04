#pragma once
#include <cmath>
#include <vector>
#include "matrix.h"

template <class T>
Matrix<T> ZigzagMatrix(const std::vector<T>& values, int size) {
    Matrix<T> matrix(size, size);
    size_t value_index = 0;
    for (int index_sum = 0; index_sum < 2 * size - 1; ++index_sum) {
        for (int row = std::max(0, index_sum - size + 1);
             row < std::min(index_sum + 1, size); ++row) {
            int real_col = index_sum - row;
            int real_row = row;
            if (index_sum % 2 == 0) {
                std::swap(real_row, real_col);
            }
            matrix(real_row, real_col) = values[value_index];
            ++value_index;
        }
    }
    return matrix;
}

template <class T, class V>
void ElemMultiply(Matrix<T>* changing_matrix, const Matrix<V>& table) {
    for (int row = 0; row < changing_matrix->Rows(); ++row) {
        for (int col = 0; col < changing_matrix->Columns(); ++col) {
            (*changing_matrix)(row, col) *= table(row, col);
        }
    }
}

// here comes political tricks

template <class T>
void PutIn(const Matrix<T>& small_matrix, Matrix<T>* big_matrix, int start_row,
           int start_col) {
    for (int row_ind = 0; row_ind < small_matrix.Rows(); ++row_ind) {
        for (int col_ind = 0; col_ind < small_matrix.Columns(); ++col_ind) {
            (*big_matrix)(start_row + row_ind, start_col + col_ind) =
                small_matrix(row_ind, col_ind);
        }
    }
}