#pragma once

#include <string>
#include <random>
#include <climits>

#include "../Helpers/StringHelper.h"

class FileId
{
public:
	FileId() { Create(); }
	explicit FileId(uint64_t fileId) { fileId_ = fileId; }

	FileId(const FileId & rhs) { fileId_ = rhs.fileId_; }
	FileId & operator=(const FileId & rhs) = default;

	FileId(FileId && rhs) noexcept { fileId_ = rhs.fileId_; }
	FileId & operator=(FileId && rhs) = default;

	bool operator==(const FileId & rhs) const {
		return fileId_ == rhs.fileId_;
	}

	bool operator!=(const FileId & rhs) const {
		return !operator==(rhs);
	}

	[[nodiscard]]
	std::string ToString() const { return to_string(fileId_); }

	[[nodiscard]]
	uint64_t Get() const { return fileId_; }
	void Set(uint64_t fileId) { fileId_ = fileId; }

private:
	uint64_t fileId_ = 0;

	void Create() {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> distr(0, ULONG_MAX);

		fileId_ = (static_cast<uint64_t>(distr(rng)) << 32) | distr(rng);
	}

};
