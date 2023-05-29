#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <chrono>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std::chrono;

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "FileId.h"
#include "Chunk.h"
#include "Tier.h"
#include "../Net/INetManager.h"
#include "../Security/Security.h"

class Unit
{
public:
	Unit() : m_fileSize(0) {}
	Unit(std::string fileName, std::uintmax_t fileSize,
		 std::shared_ptr<INetManager> netManager,
		 std::shared_ptr<Security> security);
	Unit(const Unit & other);
	Unit & operator=(const Unit & other);
	Unit(Unit && other) noexcept;
	Unit & operator=(Unit && other) noexcept;

	void Modified();

	void CheckTier();
	[[nodiscard]]
	int TierLevel() const { return m_tier.TierLevel(); }
	[[nodiscard]]
	bool TierCompliance() const { return m_tier.TierCompliance(); }
	void CompleteTier();

	bool Restore();

	[[nodiscard]]
	std::string FileDir() const { return m_fileDir; }
	[[nodiscard]]
	std::string FileName() const { return m_fileName; }
	[[nodiscard]]
	fs::path FilePath() const {
		fs::path filePath = m_fileDir;
		filePath /= m_fileName;
		return filePath;
	}
	[[nodiscard]]
	std::uintmax_t Size() const { return m_fileSize; }

	std::shared_ptr<Buffer> GetChunk(const fs::path & fileName);

private:
	std::string m_fileDir;
	std::string m_fileName;
	std::uintmax_t m_fileSize;

	FileId m_fileId;
	std::vector<Chunk> m_chunks;

	std::shared_ptr<INetManager> m_netManager;
	std::shared_ptr<Security> m_security;

	Tier m_tier;

	std::string ChangeSlash(const std::string & path);
	void LoadMeta();
	void LoadChunks(Document & doc);
	void SaveMeta();
	void SaveChunks(PrettyWriter<StringBuffer> & writer);
	bool Split();
	bool RemoveNotSplit();
	bool Send();
	std::string UploadCommand(uint32_t id);

	bool Gather();
	std::string DownloadCommand(uint32_t id);
	bool Decompress();

	fs::path ChunkName(const Chunk & chunk);
	fs::path CompressedName(const Chunk & chunk);
	fs::path DecompressedName(const Chunk & chunk);

};
