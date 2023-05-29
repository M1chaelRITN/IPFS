#include "Encryption.h"

#include <algorithm>

bool Encryption::inited_ = false;

Encryption::Encryption()
          : ecb_(nullptr)
{
	Init();
}

Encryption::~Encryption()
{
	Clear();
}

void Encryption::Clear()
{
	if (ecb_)
	{
		ecb_done(ecb_);
		delete ecb_;
		ecb_ = nullptr;
	}
}

int Encryption::SetKey(const std::string & password)
{
	if (password.empty()) { return 0; }

	std::string alg = "aes";
	int cryptBits = 256;
	int req_key_length = (cryptBits / 8);
	int cipher_idx = find_cipher(alg.c_str());
	if (cipher_idx == -1) { return -1; }

	req_key_length = std::clamp(req_key_length, cipher_descriptor[cipher_idx].min_key_length, cipher_descriptor[cipher_idx].max_key_length);

	int hash_idx = find_hash("md5");
	if (hash_idx == -1 || req_key_length > (int)hash_descriptor[hash_idx].hashsize)
	{
		hash_idx = find_hash("sha256");
		if (hash_idx == -1 || req_key_length > (int)hash_descriptor[hash_idx].hashsize)
		{
			hash_idx = find_hash("sha512");
			if (req_key_length > (int)hash_descriptor[hash_idx].hashsize) { return -1; }
		}
	}

	unsigned char key[MAXBLOCKSIZE];
	int rc;
	unsigned long outlen = sizeof(key);
	if (rc = hash_memory(hash_idx, (const uint8_t *)password.c_str(), (unsigned long)password.length(), key, &outlen); rc != CRYPT_OK)
	{
		return -1;
	}

	ecb_ = new symmetric_ECB;
	if (rc = ecb_start(cipher_idx, key, req_key_length, 0, ecb_); rc != CRYPT_OK)
	{
		Clear();
		return -1;
	}

	return 0;
}

int Encryption::Encrypt(std::vector<uint8_t> & dataBuffer, int inputSize)
{
	if (!IsReady()) { return -1; }

	int ttlInputSize;
	int req_size = ttlInputSize = inputSize + si;
	req_size = EncryptDataGetReqSize(req_size);
	if (req_size < 0) { return -1; }

	// if (req_size > outputSize) { return -1; }

	memcpy(&dataBuffer[req_size - si], &ttlInputSize, si);
	int rc = ecb_encrypt(dataBuffer.data(), dataBuffer.data(), req_size, ecb_);
	if (rc != CRYPT_OK) { return -1; }

	return req_size;
}

int Encryption::EncryptDataGetReqSize(int size)
{
	if (!IsReady()) { return -1; }

	int rem = size % ecb_->blocklen;
	if (!rem) { return -1; }

	size += ecb_->blocklen - rem;

	return size;
}

int Encryption::Decrypt(std::vector<uint8_t> & data, int inputSize)
{
	if (!IsReady() || data.empty() || inputSize == 0) { return -1; }

	int rc = ecb_decrypt(data.data(), data.data(), inputSize, ecb_);
	if (rc != CRYPT_OK) { return -1; }

	int ttlInputSize;
	memcpy(&ttlInputSize, &data[inputSize - si], si);

	return ttlInputSize - si;
}

void Encryption::Init()
{
	if (!inited_)
	{
		register_cipher (&aes_desc);
		register_hash(&md5_desc);
		register_hash(&sha256_desc);
		register_hash(&sha512_desc);

		register_prng(&yarrow_desc);
		register_prng(&sprng_desc);

		inited_ = true;
	}
}
