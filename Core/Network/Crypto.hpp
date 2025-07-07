#pragma once

/**
 * @file Crypto.hpp
 * @brief AES-256-CBC encrypt/decrypt helper.
 */

#include <openssl/evp.h>
#include <vector>
#include <cstdint>

/**
 * @class Crypto
 * @brief Provides AES-256-CBC encryption and decryption for packet payloads.
 */
class Crypto
{
public:
	/**
	 * @brief Constructor with key and IV.
	 * @param key 32 bytes (256-bit).
	 * @param iv 16 bytes (128-bit).
	 */
	Crypto(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv)
		: key_(key), iv_(iv)
	{
	}

	  /**
	   * @brief Encrypt raw data with AES-256-CBC.
	   * @param data Plain bytes.
	   * @return Encrypted bytes.
	   */
	std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data)
	{
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		std::vector<uint8_t> ciphertext(data.size() + EVP_MAX_BLOCK_LENGTH);

		int len;
		int ciphertext_len;

		EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data());
		EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size());
		ciphertext_len = len;
		EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
		ciphertext_len += len;

		EVP_CIPHER_CTX_free(ctx);
		ciphertext.resize(ciphertext_len);
		return ciphertext;
	}

	/**
	 * @brief Decrypt AES-256-CBC encrypted data.
	 * @param ciphertext Encrypted bytes.
	 * @return Decrypted plain bytes.
	 */
	std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext)
	{
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		std::vector<uint8_t> plaintext(ciphertext.size());

		int len;
		int plaintext_len;

		EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.data(), iv_.data());
		EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
		plaintext_len = len;
		EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
		plaintext_len += len;

		EVP_CIPHER_CTX_free(ctx);
		plaintext.resize(plaintext_len);
		return plaintext;
	}

private:
	std::vector<uint8_t> key_; /**< AES key (32 bytes). */
	std::vector<uint8_t> iv_;  /**< AES IV (16 bytes). */
};
