#include <climits>
#include <cstddef>
#include <iostream>
#include <stdexcept>

class LogicalValuesArray {
private:
    unsigned int value_;

public:
    explicit LogicalValuesArray(unsigned int value = 0) : value_(value) {}
    LogicalValuesArray(const LogicalValuesArray&) = default;
    LogicalValuesArray(LogicalValuesArray&&) noexcept = default;
    LogicalValuesArray& operator=(const LogicalValuesArray&) = default;
    LogicalValuesArray& operator=(LogicalValuesArray&&) noexcept = default;
    ~LogicalValuesArray() = default;

    unsigned int value() const { return value_; }
    LogicalValuesArray inversion() const { return LogicalValuesArray(~value_); }
    LogicalValuesArray conjunction(const LogicalValuesArray& other) const { return LogicalValuesArray(value_ & other.value_); }
    LogicalValuesArray disjunction(const LogicalValuesArray& other) const { return LogicalValuesArray(value_ | other.value_); }
    LogicalValuesArray implication(const LogicalValuesArray& other) const { return LogicalValuesArray((~value_) | other.value_); }
    LogicalValuesArray coimplication(const LogicalValuesArray& other) const { return LogicalValuesArray(value_ | (~other.value_)); }
    LogicalValuesArray xor_value(const LogicalValuesArray& other) const { return LogicalValuesArray(value_ ^ other.value_); }
    LogicalValuesArray equivalence(const LogicalValuesArray& other) const { return LogicalValuesArray(~(value_ ^ other.value_)); }
    LogicalValuesArray pierce(const LogicalValuesArray& other) const { return LogicalValuesArray(~(value_ | other.value_)); }
    LogicalValuesArray sheffer(const LogicalValuesArray& other) const { return LogicalValuesArray(~(value_ & other.value_)); }

    static bool equals(const LogicalValuesArray& left, const LogicalValuesArray& right) {
        return left.value_ == right.value_;
    }

    bool get_bit(std::size_t position) const {
        const std::size_t bits = sizeof(unsigned int) * CHAR_BIT;
        if (position >= bits) {
            throw std::out_of_range("Bit position is out of range");
        }
        return ((value_ >> position) & 1U) != 0;
    }

    bool operator[](std::size_t position) const { return get_bit(position); }

    void to_c_string(char* result) const {
        if (result == nullptr) {
            throw std::invalid_argument("Output buffer is null");
        }
        const std::size_t bits = sizeof(unsigned int) * CHAR_BIT;
        for (std::size_t i = 0; i < bits; ++i) {
            const std::size_t bit = bits - 1 - i;
            result[i] = ((value_ >> bit) & 1U) ? '1' : '0';
        }
        result[bits] = '\0';
    }
};

int main() {
    try {
        LogicalValuesArray a(10U);
        LogicalValuesArray b(12U);
        char text[sizeof(unsigned int) * CHAR_BIT + 1];

        a.to_c_string(text);
        std::cout << "a:       " << text << '\n';
        a.conjunction(b).to_c_string(text);
        std::cout << "a AND b: " << text << '\n';
        a.disjunction(b).to_c_string(text);
        std::cout << "a OR b:  " << text << '\n';
        std::cout << "Bit 1 of a: " << a[1] << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}

