#pragma once

#include <string>
#include <cstring>
#include <vector>

class Base64
{
	class TempBucket
	{
	public:
		uint8_t nData[4] = { 0 };
		uint8_t nSize = 0;
		void Clear() {
			memset(nData, 0, sizeof(nData)); nSize = 0;
		};
	};

public:
	Base64() = default;
	virtual ~Base64() = default;

	void Encode(const std::vector<uint8_t> & buffer, uint32_t len);
	void Encode(std::string & message);
	void Decode(const std::vector<uint8_t> & buffer, int * count = nullptr);
	void Decode(std::string & message, int * count = nullptr);

	[[nodiscard]]
	virtual std::vector<uint8_t> DecodedMessage() const { return decBuffer_; }
	[[nodiscard]]
	virtual std::vector<uint8_t> EncodedMessage() const { return encBuffer_; }

private:
	void EncodeToBuffer(const TempBucket & decodeData, uint8_t * buffer);
	uint32_t DecodeToBuffer(const TempBucket & decodeData, uint8_t * buffer);
	void EncodeRaw(TempBucket & data, const TempBucket & decodeData);
	void DecodeRaw(TempBucket & data, const TempBucket & decodeData);
	std::vector<uint8_t> Load(const std::vector<uint8_t> & data);

	static char decodeTable_[256];
	static bool inited_;
	void Init();

	std::vector<uint8_t> decBuffer_;
	std::vector<uint8_t> encBuffer_;
	uint32_t  nDDataLen_ = 0;
	uint32_t  nEDataLen_ = 0;

};
