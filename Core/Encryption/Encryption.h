#pragma once

#include <cstdlib>
#include <string>
#include <vector>

#define LTC_NO_PROTOTYPES
#include "tomcrypt.h"

class Encryption
{
public:
	Encryption();
	virtual ~Encryption();

	int Encrypt(std::vector<uint8_t> & dataBuffer, int inputSize);
	int Decrypt(std::vector<uint8_t> & dataBuffer, int inputSize);

	[[nodiscard]]
	bool IsReady() const { return ecb_ != nullptr; }

	int  SetKey(const std::string & password);

	int  EncryptDataGetReqSize(int size);

private:
	constexpr static int si = static_cast<int>(sizeof(int));
	static bool inited_;
	symmetric_ECB * ecb_;

	static void Init();
	void Clear();

};
