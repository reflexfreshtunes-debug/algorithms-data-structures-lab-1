#include <cmath>
#include <iostream>
#include <stdexcept>

class Complex {
private:
    double real_;
    double imaginary_;

public:
    Complex(double real = 0.0, double imaginary = 0.0) : real_(real), imaginary_(imaginary) {}
    Complex(const Complex&) = default;
    Complex(Complex&&) noexcept = default;
    Complex& operator=(const Complex&) = default;
    Complex& operator=(Complex&&) noexcept = default;
    ~Complex() = default;

    Complex& operator+=(const Complex& other) {
        real_ += other.real_;
        imaginary_ += other.imaginary_;
        return *this;
    }

    Complex& operator-=(const Complex& other) {
        real_ -= other.real_;
        imaginary_ -= other.imaginary_;
        return *this;
    }

    Complex& operator*=(const Complex& other) {
        const double new_real = real_ * other.real_ - imaginary_ * other.imaginary_;
        const double new_imaginary = real_ * other.imaginary_ + imaginary_ * other.real_;
        real_ = new_real;
        imaginary_ = new_imaginary;
        return *this;
    }

    Complex& operator/=(const Complex& other) {
        const double denominator = other.real_ * other.real_ + other.imaginary_ * other.imaginary_;
        if (std::fabs(denominator) < 1e-12) {
            throw std::domain_error("Division by zero complex number");
        }
        const double new_real = (real_ * other.real_ + imaginary_ * other.imaginary_) / denominator;
        const double new_imaginary = (imaginary_ * other.real_ - real_ * other.imaginary_) / denominator;
        real_ = new_real;
        imaginary_ = new_imaginary;
        return *this;
    }

    Complex operator+(const Complex& other) const { Complex result(*this); return result += other; }
    Complex operator-(const Complex& other) const { Complex result(*this); return result -= other; }
    Complex operator*(const Complex& other) const { Complex result(*this); return result *= other; }
    Complex operator/(const Complex& other) const { Complex result(*this); return result /= other; }

    double modulus() const { return std::sqrt(real_ * real_ + imaginary_ * imaginary_); }
    double argument() const { return std::atan2(imaginary_, real_); }

    friend std::ostream& operator<<(std::ostream& output, const Complex& value) {
        output << value.real_ << (value.imaginary_ < 0 ? " - " : " + ") << std::fabs(value.imaginary_) << 'i';
        return output;
    }

    friend std::istream& operator>>(std::istream& input, Complex& value) {
        double real = 0.0;
        double imaginary = 0.0;
        if (input >> real >> imaginary) {
            value.real_ = real;
            value.imaginary_ = imaginary;
        }
        return input;
    }
};

int main() {
    Complex a(3.0, 4.0);
    Complex b(1.0, -2.0);
    std::cout << "a = " << a << '\n';
    std::cout << "b = " << b << '\n';
    std::cout << "a + b = " << a + b << '\n';
    std::cout << "a * b = " << a * b << '\n';
    std::cout << "|a| = " << a.modulus() << ", arg(a) = " << a.argument() << '\n';
    return 0;
}
