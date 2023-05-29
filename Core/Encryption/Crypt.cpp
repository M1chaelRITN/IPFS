#include "Crypt.h"

using namespace std;

Crypt::Crypt() {
	gen_.seed(rd_());
	gen_simples();
	gen_keys();
}

string Crypt::encrypt(const string message)
{
	return encrypt(message, open_key_);
}

string Crypt::encrypt(const string message, const std::vector<int> & key)
{
	stringstream ss;

	size_t index = 0;
	for (size_t c = 0; c < message.length(); ++c)
	{
		char ch = message[c];
		int e = key[index];
		int n = key[index + 1];
		index += 2;
		if (index >= key.size()) { index = 0; }

		auto crypted = crypt_char(ch, e, n);
		ss << crypted;
	}

	return ss.str();
}

string Crypt::decrypt(const string message)
{
	stringstream ss;

	size_t index = 0;
	for (size_t c = 0; c < message.length(); c += 4) {
		int d = priv_key_[index];
		int n = priv_key_[index + 1];
		index += 2;
		if (index >= priv_key_.size()) { index = 0; }

		int sym = 0;
		for (int i = 0; i < 4; ++i) {
			sym |= (((int)(uint8_t)message[c + i]) << (i << 3));
		}
		auto decrypted = decrypt_char(sym, d, n);
		ss << decrypted;
	}

	return ss.str();
}

int Crypt::random(int low, int high)
{
	std::uniform_int_distribution<> dist(low, high);
	return dist(gen_);
}

void Crypt::gen_simples()
{
	auto is_simple = [&](int num) {
		for (auto & sim : simples_) {
			if (sim > 1 && num % sim == 0) {
				return false;
			}
		}
		return true;
	};

	for (int n = 1; n < 256; ++n) {
		if (is_simple(n)) {
			simples_.push_back(n);
		}
	}
}

void Crypt::gen_keys()
{
	int p, s, n, f, e, k, d;

	for (int i = 0; i < 100; i++) {
		p = get_simple();
		s = get_simple(p);
		n = p * s;
		f = (p - 1) * (s - 1);
		get_coeff(e, k, f);
		d = (k * f + 1) / e;

		open_key_.push_back(e); open_key_.push_back(n);
		priv_key_.push_back(d); priv_key_.push_back(n);
	}
}

int Crypt::get_simple(int prev /*= 0*/)
{
	int index = random(5, (int)(simples_.size() - 1));
	int simple = simples_[index];
	if (prev != 0) {
		while (simple * prev < 255) {
			index++;
			simple = simples_[index];
		}
		if (simple == prev) {
			index++;
			simple = simples_[index];
		}
	}

	return simple;
}

void Crypt::get_coeff(int & e, int & k, int f)
{
	k = 20;
	for (int i = 0; i < 10; i++) {
		e = 2;
		for (int j = 0; j < 10; j++) {
			if ((k * f + 1) % e == 0) {
				return;
			}
			e++;
		}
		k--;
		if (k == 0) {
			e = 1;
			return;
		}
	}
}

string Crypt::crypt_char(uint8_t ch, int e, int n)
{
	stringstream ss;

	long sym = 1;
	for (int i = 0; i < e; ++i) {
		sym *= ch;
		sym %= n;
	}

	for (int i = 0; i < 4; ++i) {
		ss << (char)(uint8_t)((sym >> (i << 3)) & 0xff);
	}

	return ss.str();
}

char Crypt::decrypt_char(int ch, int d, int n)
{
	long sym = 1;
	for (int i = 0; i < d; ++i) {
		sym *= ch;
		sym %= n;
	}

	return (char)(uint8_t)sym;
}
