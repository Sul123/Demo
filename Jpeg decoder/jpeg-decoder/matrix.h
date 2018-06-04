#pragma once
#include <vector>

template <class T>
class Matrix {
public:
    Matrix() = default;

    explicit Matrix(size_t rows, size_t columns)
        : matrix_(rows * columns),
          rows_(rows),
          columns_(columns) {}

    Matrix(const Matrix& other)
        : matrix_(other.matrix_),
          rows_(other.rows_),
          columns_(other.columns_) {}

    Matrix& operator=(Matrix other) {
        matrix_ = other.matrix_;
        rows_ = other.rows_;
        columns_ = other.columns_;
        return *this;
    }

    const T& operator()(int row, int col) const {
        return matrix_[row * columns_ + col];
    }

    T& operator()(int row, int col) {
        return matrix_[row * columns_ + col];
    }

    void Resize(size_t rows, size_t columns) {
        matrix_.resize(rows * columns);
        rows_ = rows;
        columns_ = columns;
    }

    size_t Rows() const {
        return rows_;
    }

    size_t Columns() const {
        return columns_;
    }


private:
    std::vector<T> matrix_;
    size_t rows_;
    size_t columns_;
};
