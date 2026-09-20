#include <fstream>
#include <iostream>
#include <new>
#include <stdexcept>

class Encoder {
private:
    unsigned char* key_;
    std::size_t key_size_;

public:
    Encoder(const unsigned char* key, std::size_t size) : key_(nullptr), key_size_(0) {
        set_key(key, size);
    }

    Encoder(const Encoder& other) : key_(nullptr), key_size_(0) {
        set_key(other.key_, other.key_size_);
    }

    Encoder(Encoder&& other) noexcept : key_(other.key_), key_size_(other.key_size_) {
        other.key_ = nullptr;
        other.key_size_ = 0;
    }

    Encoder& operator=(const Encoder& other) {
        if (this != &other) {
            set_key(other.key_, other.key_size_);
        }
        return *this;
    }

    Encoder& operator=(Encoder&& other) noexcept {
        if (this != &other) {
            delete[] key_;
            key_ = other.key_;
            key_size_ = other.key_size_;
            other.key_ = nullptr;
            other.key_size_ = 0;
        }
        return *this;
    }

    ~Encoder() {
        delete[] key_;
    }

    void set_key(const unsigned char* key, std::size_t size) {
        if (key == nullptr || size == 0) {
            throw std::invalid_argument("Key must not be empty");
        }
        unsigned char* new_key = new unsigned char[size];
        for (std::size_t i = 0; i < size; ++i) {
            new_key[i] = key[i];
        }
        delete[] key_;
        key_ = new_key;
        key_size_ = size;
    }

    bool encode(const char* input_path, const char* output_path, bool encrypt) const {
        (void)encrypt; // RC4 performs encryption and decryption in the same way.
        if (input_path == nullptr || output_path == nullptr) {
            return false;
        }

        std::ifstream input(input_path, std::ios::binary);
        if (!input) {
            return false;
        }
        std::ofstream output(output_path, std::ios::binary);
        if (!output) {
            return false;
        }

        unsigned char state[256];
        for (int i = 0; i < 256; ++i) {
            state[i] = static_cast<unsigned char>(i);
        }

        int j = 0;
        for (int i = 0; i < 256; ++i) {
            j = (j + state[i] + key_[static_cast<std::size_t>(i) % key_size_]) % 256;
            unsigned char temp = state[i];
            state[i] = state[j];
            state[j] = temp;
        }

        int i = 0;
        j = 0;
        char byte = 0;
        while (input.get(byte)) {
            i = (i + 1) % 256;
            j = (j + state[i]) % 256;
            unsigned char temp = state[i];
            state[i] = state[j];
            state[j] = temp;
            unsigned char gamma = state[(state[i] + state[j]) % 256];
            char result = static_cast<char>(static_cast<unsigned char>(byte) ^ gamma);
            output.put(result);
            if (!output) {
                return false;
            }
        }
        return input.eof();
    }
};

int main(int argc, char* argv[]) {
    try {
        const unsigned char key[] = {'s', 'i', 'm', 'p', 'l', 'e'};
        Encoder encoder(key, sizeof(key));

        if (argc == 4) {
            if (!encoder.encode(argv[1], argv[2], true) ||
                !encoder.encode(argv[2], argv[3], false)) {
                std::cerr << "File processing error\n";
                return 1;
            }
            std::cout << "The file was encrypted and decrypted\n";
            return 0;
        }
        if (argc != 1) {
            std::cerr << "Usage: task_01 [input encrypted_output decrypted_output]\n";
            return 1;
        }

        std::ofstream sample("sample.txt", std::ios::binary);
        if (!sample) {
            std::cerr << "Cannot create sample.txt\n";
            return 1;
        }
        sample << "RC4 example";
        sample.close();

        if (!encoder.encode("sample.txt", "encrypted.bin", true) ||
            !encoder.encode("encrypted.bin", "decrypted.txt", false)) {
            std::cerr << "File processing error\n";
            return 1;
        }
        std::cout << "Created encrypted.bin and decrypted.txt\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}

