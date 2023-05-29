#include "EncryptedKey.h"

#include "Base64.h"
#include "Encryption.h"
#include "../Helpers/StringHelper.h"

std::string encKey = "6ivgfh-386y6ivg-fh386y6ivg,6ivgfh3-86y6iv";
std::string decKey = "86y6iv-6ivgfh3-fh386y6ivg,386y6ivg-6ivgfh";

int EncryptedKey::EncryptKey(std::string & key, bool encrypt)
{
	int hr = 0;
	try
	{
		Encryption encryption;
		hr = encryption.SetKey(encrypt ? encKey : decKey);
		if (hr) { return hr; }

		std::vector<uint8_t> data = stov(key);
		int iOutputSize = encryption.Encrypt(data, (int)key.length());
		if (iOutputSize < 0) { return -1; }

		Base64 coder;
		coder.Encode(data, iOutputSize);
		if (iOutputSize)
		{
			key = vtos(coder.EncodedMessage());
		}
	}
	catch (int err)
	{
		hr = err;
	}
	catch (...)
	{
		hr = -1;
	}

	return hr;
}

int EncryptedKey::DecryptKey(std::string & key, bool encrypt, bool decoding /*= true*/)
{
	return DecryptKey(key, (encrypt ? encKey : decKey), decoding);
}

int EncryptedKey::DecryptKey(std::string & key, const std::string & decryptKey, bool decoding /*= true*/)
{
	int hr = 0;
	std::vector<uint8_t> encPassword;
	try
	{
		int inputSize = 0;
//		if (decoding) {
			Base64 coder;
			coder.Decode(key, &inputSize);
			if (inputSize) { encPassword = coder.DecodedMessage(); }
//		}
//		else
//		{
//			int iLen = sKey.length();
//			int nMBLen = WideCharToMultiByte(CP_ACP, 0, sKey.GetString(), iLen, NULL, 0, NULL, NULL);
//			pEncPassword = new uint8_t[nMBLen+1];
//			WideCharToMultiByte(CP_ACP, 0, sKey.c_str(), iLen, pEncPassword, nMBLen + 1, NULL, NULL);
//			iInputSize = (int)strlen((char *) pEncPassword);
//		}
		if (inputSize == 0 || encPassword.empty())
		{
			//hr = E_PASSWORD_ENCRYPTION_CORRUPTED_RT;
			//         if (m_pMessages)
			//{
			//             CAtlString msg = L"CEncryptedKey::DecryptKey - Password not decoded. Abort.";
			//             m_pMessages->ErrorMessage(msg, hr);
			//         }
			// TraceThrow(int, hr);
		}

		Encryption encryption;
		hr = encryption.SetKey(decryptKey);
		if (hr)
		{
			//         if (m_pMessages)
			//{
			//             CAtlString msg;
			//             msg.Format(L"CEncryptedKey::DecryptKey - Encryption.SetKey returned GetLastError 0x%08x. Abort.", hr);
			//             m_pMessages->ErrorMessage(msg, hr);
			//         }
			// TraceThrow(int, hr);
		}
		int outputSize = inputSize * 2;
		std::vector<uint8_t> decPassword = encPassword;
		outputSize = encryption.Decrypt(decPassword, inputSize);
		if (outputSize < 0) { return -1; }
		key = vtos(decPassword);
	}
	catch (int err)
	{
		hr = err;
	}
	catch (...)
	{
		hr = -1;
	}

	return hr;
}
