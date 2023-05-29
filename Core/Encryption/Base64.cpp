#include "Base64.h"

#include "../Helpers/StringHelper.h"

#include <algorithm>
#include <unordered_set>

static char Base64Digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool Base64::inited_ = false;
char Base64::decodeTable_[256];

#ifndef PAGE_SIZE
#  define PAGE_SIZE 4096
#endif

#ifndef ROUND_TO_PAGE
#  define ROUND_TO_PAGE(a) ((((a) / PAGE_SIZE) + 1) * PAGE_SIZE)
#endif

std::vector<uint8_t> Base64::Load(const std::vector<uint8_t> & data)
{
	std::vector<uint8_t> cleared;

	auto good_mime ([](uint8_t ch) {
		static const std::unordered_set<uint8_t> bad_chars { '\r', '\n', '\t', ' ', '\b', '\a', '\f', '\v' };
		return bad_chars.find(ch) == bad_chars.end();
	});

	std::copy_if(begin(data), end(data), std::back_inserter(cleared), good_mime);

	return cleared;
}

void Base64::Encode(const std::vector<uint8_t> & buffer, uint32_t len)
{
	decBuffer_ = buffer;
	encBuffer_.clear(); encBuffer_.resize(buffer.size() * 2, 0);

	TempBucket raw;
	uint32_t nIndex = 0;

	while ((nIndex + 3) <= len)
	{
		raw.Clear();
		memcpy(&raw, &decBuffer_[nIndex], 3);
		raw.nSize = 3;
		EncodeToBuffer(raw, &encBuffer_[nEDataLen_]);
		nIndex      += 3;
		nEDataLen_ += 4;
	}

	if (len > nIndex)
	{
		raw.Clear();
		raw.nSize = (uint8_t)(len - nIndex);
		memcpy(&raw, &decBuffer_[nIndex], len - nIndex);
		EncodeToBuffer(raw, &encBuffer_[nEDataLen_]);
		nEDataLen_ += 4;
	}
}

void Base64::Encode(std::string & message)
{
	if (message.empty()) { return; }

	Encode(stov(message), message.length());
}

void Base64::Decode(const std::vector<uint8_t> & buffer, int * count /*= nullptr*/)
{
	Init();

	encBuffer_ = Load(buffer);
	decBuffer_.clear(); decBuffer_.resize(buffer.size(), 0);

	TempBucket raw;

	uint32_t nIndex = 0;
	while ((nIndex + 4) <= nEDataLen_)
	{
		raw.Clear();
		raw.nData[0] = Base64::decodeTable_[encBuffer_[nIndex]];
		raw.nData[1] = Base64::decodeTable_[encBuffer_[nIndex + 1]];
		raw.nData[2] = Base64::decodeTable_[encBuffer_[nIndex + 2]];
		raw.nData[3] = Base64::decodeTable_[encBuffer_[nIndex + 3]];

		if (count) { *count += 3; }

		if (raw.nData[2] == 255)
		{
			if (count) { --*count; }
			raw.nData[2] = 0;
		}
		if (raw.nData[3] == 255)
		{
			if (count) { --*count; }
			raw.nData[3] = 0;
		}

		raw.nSize = 4;
		DecodeToBuffer(raw, &decBuffer_[nDDataLen_]);
		nIndex += 4;
		nDDataLen_ += 3;
	}
	// Если nIndex < m_nEDataLen, то мы получили декодированное сообщение без заполнения.
	// Мы можем захотеть выдать здесь какое-то предупреждение, но нам все равно необходимо
	// для обработки декодирования, как если бы оно было правильно дополнено.
	if (nIndex < nEDataLen_)
	{
		raw.Clear();
		for (uint32_t i = nIndex; i < nEDataLen_; i++)
		{
			raw.nData[i - nIndex] = Base64::decodeTable_[encBuffer_[i]];
			raw.nSize++;
			if (raw.nData[i - nIndex] == 255) { raw.nData[i - nIndex] = 0; }
		}

		DecodeToBuffer(raw, &decBuffer_[nDDataLen_]);
		nDDataLen_ += (nEDataLen_ - nIndex);
	}
}

void Base64::Decode(std::string & message, int * count /*= nullptr*/)
{
	if (message.empty()) { return; }

	std::vector<uint8_t> data = stov(message);
	Decode(data, count);
}

void Base64::EncodeToBuffer(const TempBucket & decodeData, uint8_t * buffer)
{
	TempBucket data;

	EncodeRaw(data, decodeData);

	for (int i = 0; i < 4; i++)
		buffer[i] = Base64Digits[data.nData[i]];

	switch (decodeData.nSize)
	{
	case 1: buffer[2] = '=';
	case 2: buffer[3] = '=';
	}
}

uint32_t Base64::DecodeToBuffer(const TempBucket & decodeData, uint8_t * buffer)
{
	TempBucket data;
	uint32_t nCount = 0;

	DecodeRaw(data, decodeData);

	for (int i = 0; i < 3; i++)
	{
		buffer[i] = data.nData[i];
		if (buffer[i] != 255)
			nCount++;
	}

	return nCount;
}

void Base64::EncodeRaw(TempBucket & data, const TempBucket & decodeData)
{
	uint8_t nTemp;

	data.nData[0] = decodeData.nData[0];
	data.nData[0] >>= 2;

	data.nData[1] = decodeData.nData[0];
	data.nData[1] <<= 4;
	nTemp = decodeData.nData[1];
	nTemp >>= 4;
	data.nData[1] |= nTemp;
	data.nData[1] &= 0x3F;

	data.nData[2] = decodeData.nData[1];
	data.nData[2] <<= 2;

	nTemp = decodeData.nData[2];
	nTemp >>= 6;

	data.nData[2] |= nTemp;
	data.nData[2] &= 0x3F;

	data.nData[3] = decodeData.nData[2];
	data.nData[3] &= 0x3F;
}

void Base64::DecodeRaw(TempBucket & data, const TempBucket & decodeData)
{
	uint8_t nTemp;

	data.nData[0] = decodeData.nData[0];
	data.nData[0] <<= 2;

	nTemp = decodeData.nData[1];
	nTemp >>= 4;
	nTemp &= 0x03;
	data.nData[0] |= nTemp;

	data.nData[1] = decodeData.nData[1];
	data.nData[1] <<= 4;

	nTemp = decodeData.nData[2];
	nTemp >>= 2;
	nTemp &= 0x0F;
	data.nData[1] |= nTemp;

	data.nData[2] = decodeData.nData[2];
	data.nData[2] <<= 6;
	nTemp = decodeData.nData[3];
	nTemp &= 0x3F;
	data.nData[2] |= nTemp;
}

void Base64::Init()
{
	if (!Base64::inited_)
	{
		int i;

		for (i = 0; i < 256; i++)
			Base64::decodeTable_[i] = -2;

		for (i = 0; i < 64; i++)
		{
			Base64::decodeTable_[Base64Digits[i]]         = (char)i;
			Base64::decodeTable_[Base64Digits[i] | 0x80]  = (char)i;
		}

		Base64::decodeTable_['='] = -1;
		Base64::decodeTable_['=' | 0x80] = -1;

		Base64::inited_ = true;
	}
}
