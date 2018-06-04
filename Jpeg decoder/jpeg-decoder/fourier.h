#pragma once
#include <cmath>
#include <cstring>
#include "fftw3.h"
#include "matrix.h"

Matrix<double> DCT(const Matrix<double>& block, fftw_plan_s* plan,
                   double* in_buffer, double* out_buffer) {
    int height = block.Rows();
    int width = block.Columns();
    Matrix<double> result(height, width);

    size_t buffer_size = height * width * sizeof(double);

    std::memcpy(in_buffer, &block(0, 0), buffer_size);

    for (int ind = 0; ind < width; ++ind) {
        in_buffer[ind] *= std::sqrt(2);
    }
    for (int ind = 0; ind < height * width; ind += width) {
        in_buffer[ind] *= std::sqrt(2);
    }

    fftw_execute(plan);

    std::memcpy(&result(0, 0), out_buffer, buffer_size);

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            result(row, col) /= 16;
        }
    }
    return result;
}
