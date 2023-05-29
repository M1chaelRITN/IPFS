#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <iterator>
#include <algorithm>
#include <exception>
#include <vector>

class Buffer
{
public:
	Buffer() = default;
	explicit Buffer(size_t size) { m_bytes.resize(size, 0); }
	explicit Buffer(std::vector<char> & bytes) : m_bytes(bytes) {}
	explicit Buffer(const std::vector<int> & bytes) {
		char * data = (char *)bytes.data();
		size_t size = bytes.size() * sizeof(int);
		std::copy(data, data + size, std::back_inserter(m_bytes));
	}
	explicit Buffer(const std::string & str) {
		std::copy(str.begin(), str.end(), m_bytes.begin());
	}
	Buffer(const Buffer & other) = default;
	Buffer(Buffer && other) noexcept : m_bytes(std::move(other.m_bytes)) {
		other.m_bytes.clear();
	}
	~Buffer() { m_bytes.clear(); }

	[[nodiscard]]
	size_t Size() const { return m_bytes.size(); }
	[[nodiscard]]
	const char * Bytes() const { return m_bytes.data(); }

	void Add(std::vector<char> & bytes, size_t size)
	{
		std::vector<char> new_bytes(m_bytes.size() + size);
		std::move(m_bytes.begin(), m_bytes.end(), new_bytes.begin());
		long l_size = static_cast<long>(size);
		long l_b_size = static_cast<long>(m_bytes.size());
		std::move(bytes.begin(), bytes.begin() + l_size, new_bytes.begin() + l_b_size);
		m_bytes = std::move(new_bytes);
	}

private:
	std::vector<char> m_bytes;

};
