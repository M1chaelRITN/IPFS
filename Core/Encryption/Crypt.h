#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <random>

// Asyncronus encryption
class Crypt
{
public:
	Crypt();

	std::string encrypt(const std::string message);
	std::string encrypt(const std::string message, const std::vector<int> & key);
	std::string decrypt(const std::string message);

	[[nodiscard]]
	const std::vector<int> open_key() const { return open_key_; }
	[[nodiscard]]
	const std::vector<int> priv_key() const { return priv_key_; }

private:
	std::vector<int> simples_;
	std::vector<int> open_key_;
	std::vector<int> priv_key_;

	std::random_device rd_;
	std::mt19937 gen_;

	int random(int low, int high);
	void gen_simples();
	void gen_keys();
	int get_simple(int prev = 0);
	void get_coeff(int & e, int & k, int f);
	std::string crypt_char(uint8_t ch, int e, int n);
	char decrypt_char(int ch, int d, int n);

};
