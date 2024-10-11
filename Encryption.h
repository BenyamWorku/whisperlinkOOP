// // Encryption.h
// #pragma once
// #include <sodium.h>
// #include <string>
// #include <vector>

// class Encryption {
// private:
//     unsigned char key[crypto_secretbox_KEYBYTES];
//     unsigned char nonce[crypto_secretbox_NONCEBYTES];

// public:
//     Encryption();
//     std::vector<unsigned char> encrypt_message(const std::string &message);
//     std::string decrypt_message(const std::vector<unsigned char> &ciphertext);
//     void print_hex(const unsigned char* data, size_t len);
// };