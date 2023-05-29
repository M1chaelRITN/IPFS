#pragma once

#include <string>

class EncryptedKey
{
public:
	EncryptedKey() = default;
	~EncryptedKey() = default;

	int EncryptKey(std::string & key, bool encrypt);
	int DecryptKey(std::string & key, bool encrypt, bool decoding = true);

private:
	int DecryptKey(std::string & key, const std::string & decKey, bool decoding = true);

};
