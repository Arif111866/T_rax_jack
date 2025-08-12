#include <iostream>
#include <string>
#include <sstream>

std::string derive_key(const std::string& key) {
    std::string derived = key;
    // Same key stretching as in encrypt
    for (int i = 0; i < 1000; ++i) {
        std::string temp = derived;
        for (size_t j = 0; j < temp.length(); ++j) {
            temp[j] ^= derived[(j + 1) % derived.length()];
        }
        derived += temp;
        if (derived.length() > 64) derived = derived.substr(0, 64);
    }
    return derived;
}

std::string decrypt(const std::string& encrypted_hex, const std::string& key) {
    if (key.empty()) throw std::invalid_argument("Key cannot be empty");
    if (encrypted_hex.length() < 32 || encrypted_hex.length() % 2 != 0) {
        throw std::invalid_argument("Invalid hex string length");
    }

    // Derive stronger key
    std::string derived_key = derive_key(key);

    // Extract salt (first 32 hex chars = 16 bytes)
    std::string salt_hex = encrypted_hex.substr(0, 32);
    std::string ciphertext_hex = encrypted_hex.substr(32);

    // Convert salt to bytes
    std::string salt;
    for (size_t i = 0; i < salt_hex.length(); i += 2) {
        std::string byte_str = salt_hex.substr(i, 2);
        try {
            salt += static_cast<char>(std::stoi(byte_str, nullptr, 16));
        } catch (...) {
            throw std::invalid_argument("Invalid hex string in salt");
        }
    }

    // Combine salt with key
    std::string salted_key = derived_key + salt;

    // Convert ciphertext to bytes and decrypt
    std::string decrypted;
    for (size_t i = 0; i < ciphertext_hex.length(); i += 2) {
        std::string byte_str = ciphertext_hex.substr(i, 2);
        try {
            unsigned char byte = static_cast<unsigned char>(std::stoi(byte_str, nullptr, 16));
            unsigned char c = byte ^ static_cast<unsigned char>(salted_key[i / 2 % salted_key.length()]);
            decrypted += static_cast<char>(c);
        } catch (...) {
            throw std::invalid_argument("Invalid hex string in ciphertext");
        }
    }
    return decrypted;
}

int main() {
    const std::string FIXED_KEY = "Arif_30";
    std::string encrypted_hex;
    std::cerr << "Enter the encrypted hex string:" << std::endl;
    std::getline(std::cin, encrypted_hex);
    try {
        std::string decrypted = decrypt(encrypted_hex, FIXED_KEY);
        std::cout << decrypted << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}