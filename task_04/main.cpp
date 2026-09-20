#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>

class Matrix {
public:
    class CalculationError : public std::exception {
    public:
        const char* what() const noexcept override { return "Matrix operation cannot be performed"; }
    };

    class Row {
    private:
        double* data_;
        std::size_t columns_;
    public:
        Row(double* data, std::size_t columns) : data_(data), columns_(columns) {}
        double& operator[](std::size_t column) {
            if (column >= columns_) throw std::out_of_range("Column is out of range");
            return data_[column];
        }
    };

    class ConstRow {
    private:
        const double* data_;
        std::size_t columns_;
    public:
        ConstRow(const double* data, std::size_t columns) : data_(data), columns_(columns) {}
        const double& operator[](std::size_t column) const {
            if (column >= columns_) throw std::out_of_range("Column is out of range");
            return data_[column];
        }
    };

private:
    double** data_;
    std::size_t rows_;
    std::size_t columns_;

    void clear() noexcept {
        for (std::size_t i = 0; i < rows_; ++i) delete[] data_[i];
        delete[] data_;
        data_ = nullptr;
        rows_ = 0;
        columns_ = 0;
    }

    void allocate(std::size_t rows, std::size_t columns) {
        if (rows == 0 || columns == 0) throw std::invalid_argument("Matrix dimensions must be positive");
        double** new_data = new double*[rows];
        std::size_t created = 0;
        try {
            for (; created < rows; ++created) {
                new_data[created] = new double[columns]();
            }
        } catch (...) {
            for (std::size_t i = 0; i < created; ++i) delete[] new_data[i];
            delete[] new_data;
            throw;
        }
        data_ = new_data;
        rows_ = rows;
        columns_ = columns;
    }

    void swap(Matrix& other) noexcept {
        double** data = data_; data_ = other.data_; other.data_ = data;
        std::size_t rows = rows_; rows_ = other.rows_; other.rows_ = rows;
        std::size_t columns = columns_; columns_ = other.columns_; other.columns_ = columns;
    }

public:
    Matrix(std::size_t rows, std::size_t columns) : data_(nullptr), rows_(0), columns_(0) {
        allocate(rows, columns);
    }

    Matrix(const Matrix& other) : data_(nullptr), rows_(0), columns_(0) {
        allocate(other.rows_, other.columns_);
        for (std::size_t i = 0; i < rows_; ++i)
            for (std::size_t j = 0; j < columns_; ++j)
                data_[i][j] = other.data_[i][j];
    }

    Matrix(Matrix&& other) noexcept : data_(other.data_), rows_(other.rows_), columns_(other.columns_) {
        other.data_ = nullptr;
        other.rows_ = 0;
        other.columns_ = 0;
    }

    Matrix& operator=(const Matrix& other) {
        if (this != &other) { Matrix copy(other); swap(copy); }
        return *this;
    }

    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            clear();
            data_ = other.data_; rows_ = other.rows_; columns_ = other.columns_;
            other.data_ = nullptr; other.rows_ = 0; other.columns_ = 0;
        }
        return *this;
    }

    ~Matrix() { clear(); }

    std::size_t rows() const { return rows_; }
    std::size_t columns() const { return columns_; }

    Row operator[](std::size_t row) {
        if (row >= rows_) throw std::out_of_range("Row is out of range");
        return Row(data_[row], columns_);
    }

    ConstRow operator[](std::size_t row) const {
        if (row >= rows_) throw std::out_of_range("Row is out of range");
        return ConstRow(data_[row], columns_);
    }

    Matrix operator+(const Matrix& other) const {
        if (rows_ != other.rows_ || columns_ != other.columns_) throw CalculationError();
        Matrix result(rows_, columns_);
        for (std::size_t i = 0; i < rows_; ++i)
            for (std::size_t j = 0; j < columns_; ++j)
                result.data_[i][j] = data_[i][j] + other.data_[i][j];
        return result;
    }

    Matrix operator-(const Matrix& other) const {
        if (rows_ != other.rows_ || columns_ != other.columns_) throw CalculationError();
        Matrix result(rows_, columns_);
        for (std::size_t i = 0; i < rows_; ++i)
            for (std::size_t j = 0; j < columns_; ++j)
                result.data_[i][j] = data_[i][j] - other.data_[i][j];
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        if (columns_ != other.rows_) throw CalculationError();
        Matrix result(rows_, other.columns_);
        for (std::size_t i = 0; i < rows_; ++i)
            for (std::size_t j = 0; j < other.columns_; ++j)
                for (std::size_t k = 0; k < columns_; ++k)
                    result.data_[i][j] += data_[i][k] * other.data_[k][j];
        return result;
    }

    Matrix operator*(double number) const {
        Matrix result(*this);
        for (std::size_t i = 0; i < rows_; ++i)
            for (std::size_t j = 0; j < columns_; ++j)
                result.data_[i][j] *= number;
        return result;
    }

    friend Matrix operator*(double number, const Matrix& matrix) { return matrix * number; }

    Matrix transpose() const {
        Matrix result(columns_, rows_);
        for (std::size_t i = 0; i < rows_; ++i)
            for (std::size_t j = 0; j < columns_; ++j)
                result.data_[j][i] = data_[i][j];
        return result;
    }

    double determinant(double epsilon = 1e-12) const {
        if (rows_ != columns_) throw CalculationError();
        Matrix copy(*this);
        double result = 1.0;
        for (std::size_t column = 0; column < columns_; ++column) {
            std::size_t pivot = column;
            for (std::size_t row = column + 1; row < rows_; ++row)
                if (std::fabs(copy.data_[row][column]) > std::fabs(copy.data_[pivot][column])) pivot = row;
            if (std::fabs(copy.data_[pivot][column]) < epsilon) return 0.0;
            if (pivot != column) {
                double* temp = copy.data_[pivot]; copy.data_[pivot] = copy.data_[column]; copy.data_[column] = temp;
                result = -result;
            }
            result *= copy.data_[column][column];
            for (std::size_t row = column + 1; row < rows_; ++row) {
                const double factor = copy.data_[row][column] / copy.data_[column][column];
                for (std::size_t j = column; j < columns_; ++j)
                    copy.data_[row][j] -= factor * copy.data_[column][j];
            }
        }
        return result;
    }

    Matrix inverse(double epsilon = 1e-12) const {
        if (rows_ != columns_) throw CalculationError();
        const std::size_t n = rows_;
        Matrix left(*this);
        Matrix right(n, n);
        for (std::size_t i = 0; i < n; ++i) right.data_[i][i] = 1.0;

        for (std::size_t column = 0; column < n; ++column) {
            std::size_t pivot = column;
            for (std::size_t row = column + 1; row < n; ++row)
                if (std::fabs(left.data_[row][column]) > std::fabs(left.data_[pivot][column])) pivot = row;
            if (std::fabs(left.data_[pivot][column]) < epsilon) throw CalculationError();
            if (pivot != column) {
                double* temp = left.data_[pivot]; left.data_[pivot] = left.data_[column]; left.data_[column] = temp;
                temp = right.data_[pivot]; right.data_[pivot] = right.data_[column]; right.data_[column] = temp;
            }
            const double divisor = left.data_[column][column];
            for (std::size_t j = 0; j < n; ++j) {
                left.data_[column][j] /= divisor;
                right.data_[column][j] /= divisor;
            }
            for (std::size_t row = 0; row < n; ++row) {
                if (row == column) continue;
                const double factor = left.data_[row][column];
                for (std::size_t j = 0; j < n; ++j) {
                    left.data_[row][j] -= factor * left.data_[column][j];
                    right.data_[row][j] -= factor * right.data_[column][j];
                }
            }
        }
        return right;
    }

    friend std::ostream& operator<<(std::ostream& output, const Matrix& matrix) {
        for (std::size_t i = 0; i < matrix.rows_; ++i) {
            for (std::size_t j = 0; j < matrix.columns_; ++j) output << matrix.data_[i][j] << '\t';
            output << '\n';
        }
        return output;
    }
};

int main() {
    try {
        Matrix matrix(2, 2);
        matrix[0][0] = 4; matrix[0][1] = 7;
        matrix[1][0] = 2; matrix[1][1] = 6;
        std::cout << "Matrix:\n" << matrix;
        std::cout << "Determinant: " << matrix.determinant() << '\n';
        std::cout << "Inverse:\n" << matrix.inverse();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
