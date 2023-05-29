#pragma once

#include <string>
#include <filesystem>
namespace fs = std::filesystem;

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "FileId.h"

struct Chunk
{
public:
	static const size_t SIZE;

	Chunk() = default;
	Chunk(const FileId & fileId, uint32_t id, bool compressed = false, bool local = true)
		: fileId_(fileId)
		, id_(id)
		, compressed_(compressed)
		, local_(local)
	{}
	Chunk(const Chunk & rhs) = default;
	Chunk & operator=(const Chunk & rhs) = default;

	Chunk(Chunk && rhs) noexcept
		: fileId_(std::move(rhs.fileId_))
		, id_(rhs.id_)
		, compressed_(rhs.compressed_)
		, local_(rhs.local_)
	{}

	Chunk & operator=(Chunk && rhs) noexcept
	{
		fileId_ = std::move(rhs.fileId_);
		id_ = rhs.id_;
		compressed_ = rhs.compressed_;
		local_ = rhs.local_;

		return *this;
	}

	bool operator==(const Chunk & rhs) const {
		return fileId_ == rhs.fileId_ && id_ == rhs.id_;
	}

	bool operator!=(const Chunk & rhs) const {
		return !operator==(rhs);
	}

	static Chunk Load(const Value & jsonChunk);
	void Save(PrettyWriter<StringBuffer> & writer);

	[[nodiscard]]
	FileId GetFileId() const { return fileId_; }
	void SetFileId(const FileId & fileId) { fileId_ = fileId; }

	[[nodiscard]]
	uint32_t Id() const { return id_; }
	void Id(uint32_t id) { id_ = id; }

	[[nodiscard]]
	bool Compressed() const { return compressed_; }
	void Compressed(bool compressed) { compressed_ = compressed; }

	[[nodiscard]]
	bool Local() const { return local_; }
	void Local(bool local) { local_ = local; }

private:
	FileId fileId_;
	uint32_t id_ = 0;
	bool compressed_ = false;
	bool local_ = true;

};
