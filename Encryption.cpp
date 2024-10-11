// // Encryption.cpp
// #include "Encryption.h"
// #include <iostream>
// #include <cstring>

// Encryption::Encryption() {
//     if (sodium_init() < 0) {
//         std::cerr << "Sodium initialization failed" << std::endl;
//         exit(1);
//     }
//     // Use a pre-shared key and nonce (NOT SECURE FOR REAL USE)
//     const char* shared_key = "mysupersecretkey0123456789012345";
//     const char* shared_nonce = "uniquenonce123";
    
//     std::memcpy(key, shared_key, crypto_secretbox_KEYBYTES);
//     std::memcpy(nonce, shared_nonce, crypto_secretbox_NONCEBYTES);
// }

// std::vector<unsigned char> Encryption::encrypt_message(const std::string &message) {
//     std::vector<unsigned char> ciphertext(crypto_secretbox_MACBYTES + message.length());
//     crypto_secretbox_easy(ciphertext.data(), reinterpret_cast<const unsigned char*>(message.data()), 
//                           message.length(), nonce, key);
//     return ciphertext;
// }

// // std::string Encryption::decrypt_message(const std::vector<unsigned char> &ciphertext) {
// //     std::vector<unsigned char> decrypted(ciphertext.size() - crypto_secretbox_MACBYTES);
// //     if (crypto_secretbox_open_easy(decrypted.data(), ciphertext.data(), ciphertext.size(), nonce, key) != 0) {
// //         return "Decryption failed";
// //     }
// //     return std::string(decrypted.begin(), decrypted.end());
// // }
// std::string Encryption::decrypt_message(const std::vector<unsigned char> &ciphertext) {
//     if (ciphertext.size() <= crypto_secretbox_MACBYTES) {
//         std::cerr << "Ciphertext size is too small to be valid." << std::endl;
//         return "Decryption failed";
//     }

//     std::vector<unsigned char> decrypted(ciphertext.size() - crypto_secretbox_MACBYTES);

//     if (crypto_secretbox_open_easy(decrypted.data(), ciphertext.data(), ciphertext.size(), nonce, key) != 0) {
//         std::cerr << "Decryption failed. Ciphertext size: " << ciphertext.size() << std::endl;
//         return "Decryption failed";
//     }

//     return std::string(decrypted.begin(), decrypted.end());
// }
