#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <random>
#include <ctime>

std::string derive_key(const std::string& key) {
    std::string derived = key;
    // Simple key stretching: repeat and XOR with shifted versions
    for (int i = 0; i < 1000; ++i) {
        std::string temp = derived;
        for (size_t j = 0; j < temp.length(); ++j) {
            temp[j] ^= derived[(j + 1) % derived.length()];
        }
        derived += temp;
        if (derived.length() > 64) derived = derived.substr(0, 64); // Cap at 64 bytes
    }
    return derived;
}

std::string generate_salt(size_t length) {
    std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i) {
        ss << std::setw(2) << (rng() % 256);
    }
    return ss.str();
}

std::string encrypt(const std::string& plaintext, const std::string& key) {
    if (key.empty()) throw std::invalid_argument("Key cannot be empty");
    if (plaintext.empty()) throw std::invalid_argument("Plaintext cannot be empty");

    // Derive stronger key
    std::string derived_key = derive_key(key);

    // Generate 16-byte (32 hex chars) salt
    std::string salt = generate_salt(16);

    // Combine salt with key for encryption
    std::string salted_key = derived_key + salt;

    // Encrypt
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    // Include salt in output
    ss << salt;
    for (size_t i = 0; i < plaintext.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(plaintext[i]) ^ static_cast<unsigned char>(salted_key[i % salted_key.length()]);
        ss << std::setw(2) << static_cast<int>(c);
    }
    return ss.str();
}

int main() {
    const std::string FIXED_KEY = "Arif_30";
    std::string plaintext;
    std::cerr << "Enter the password to encrypt:" << std::endl;
    std::getline(std::cin, plaintext);
    try {
        std::string encrypted = encrypt(plaintext, FIXED_KEY);
        std::cout << encrypted << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}