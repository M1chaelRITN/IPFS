#include "Unit.h"

#include "../Files/FileSystem.h"
#include "../Pack/PackManager.h"
#include "../Helpers/StringHelper.h"
#include "../Encryption/Encryption.h"
#include "../Security/Security.h"

#include <fstream>
using namespace std;

Unit::Unit(std::string fileName, std::uintmax_t fileSize,
		   std::shared_ptr<INetManager> netManager,
		   std::shared_ptr<Security> security)
	: m_fileName(std::move(fileName))
	, m_fileSize(fileSize)
	, m_netManager(netManager)
	, m_security(security)
{
	auto cloudFolder = FileSystem::GetCloudFolder();

	size_t rootLength = cloudFolder.string().length();
	m_fileName = ChangeSlash(m_fileName);
	if (m_fileName.substr(0, rootLength) == ChangeSlash(cloudFolder.string()))
	{
		m_fileName = m_fileName.substr(rootLength + 1);
	}

	LoadMeta();
}

Unit::Unit(const Unit & other)
	: m_fileName(other.m_fileName)
	, m_fileSize(other.m_fileSize)
	, m_fileId(other.m_fileId)
	, m_tier(other.m_tier)
	, m_chunks(other.m_chunks)
	, m_netManager(other.m_netManager)
{
}

Unit & Unit::operator=(const Unit & other)
{
	m_fileName = other.m_fileName;
	m_fileSize = other.m_fileSize;
	m_fileId = other.m_fileId;
	m_tier = other.m_tier;
	m_chunks = other.m_chunks;
	m_netManager = other.m_netManager;
	return *this;
}

Unit::Unit(Unit && other) noexcept
	: m_fileName(std::move(other.m_fileName))
	, m_fileSize(other.m_fileSize)
	, m_fileId(other.m_fileId)
	, m_tier(std::move(other.m_tier))
	, m_chunks(std::move(other.m_chunks))
	, m_netManager(std::move(other.m_netManager))
{
}

Unit & Unit::operator=(Unit && other) noexcept
{
	m_fileName = std::move(other.m_fileName);
	m_fileSize = other.m_fileSize;
	m_fileId = other.m_fileId;
	m_tier = std::move(other.m_tier);
	m_chunks = std::move(other.m_chunks);
	m_netManager = std::move(other.m_netManager);
	return *this;
}

std::string Unit::ChangeSlash(const std::string & path)
{
	std::string haystack(path);
	std::replace(haystack.begin(), haystack.end(), '\\', '/');
	return haystack;
}

void Unit::LoadMeta()
{
	fs::path metaPath = FileSystem::Combine(FileSystem::GetMetaFolder(), m_fileName);

	if (!FileSystem::Exists(metaPath)) { return; }

	auto buffer = FileSystem::ReadFile(metaPath);

	Document jsonDocument;
	jsonDocument.Parse(buffer->Bytes(), buffer->Size());

	Value & tierTimeValue = jsonDocument["tier-time"];
	int64_t timeStamp = tierTimeValue.GetInt64();

	seconds duration(timeStamp);
	auto tierTime = time_point<system_clock, seconds>(duration);

	Value & tierValue = jsonDocument["tier"];
	auto level = tierValue.GetInt();

	Value & tierCompilanceValue = jsonDocument["tier-compliance"];
	auto tierCompilance = tierCompilanceValue.GetBool();

	m_tier.Set(level, tierCompilance, tierTime);

	LoadChunks(jsonDocument);
}

void Unit::LoadChunks(Document & doc)
{
	const Value & chunks = doc["chunks"];
	assert(chunks.IsArray());

	for (SizeType i = 0; i < chunks.Size(); i++)
		m_chunks.push_back(Chunk::Load(chunks[i]));
}

void Unit::Modified()
{
	SaveMeta();
}

void Unit::SaveMeta()
{
	fs::path metaPath = FileSystem::Combine(FileSystem::GetMetaFolder(), m_fileName, true);

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);

	writer.StartObject();

	auto tierTime_s = time_point_cast<seconds>(m_tier.Time());
	auto epoch = tierTime_s.time_since_epoch();
	auto duration = duration_cast<seconds>(epoch);
	int64_t secs = duration.count();

	writer.String("tier-time");
	writer.Int64(secs);

	writer.String("tier");
	writer.Int(m_tier.TierLevel());

	writer.String("tier-compliance");
	writer.Bool(m_tier.TierCompliance());

	SaveChunks(writer);

	writer.EndObject();

	FileSystem::WriteFile(metaPath, sb.GetString(), sb.GetSize());
}

void Unit::SaveChunks(PrettyWriter<StringBuffer> & writer)
{
	writer.String("chunks");
	writer.StartArray();
	for (auto & chunk : m_chunks)
	{
		chunk.Save(writer);
	}
	writer.EndArray();
}

void Unit::CheckTier()
{
	if (m_tier.IsChangeTier()) {
		Modified();
	}
}

void Unit::CompleteTier()
{
	switch (m_tier.TierLevel()) {
	case 0: // file split to parts, every part will zip
		if (!Split()) { return; }
		break;
	case 1: // remove not splitted part
		if (!RemoveNotSplit()) { return; }
		break;
	case 2: Send(); break; // send some parts to other apps
	}

	m_tier.CompleteTier();
}

bool Unit::Split()
{
	fs::path filePath = FileSystem::Combine(FileSystem::GetCloudFolder(), m_fileName);

	size_t fileSize = FileSystem::FileSize(filePath);
	auto buffer = make_shared<Buffer>(fileSize);

	ifstream inFile;
	inFile.open(filePath, ios::binary);
	if (inFile.fail()) { return false; }

	char * bytes = (char *)(buffer->Bytes());

	size_t remainder = fileSize;
	for (uint32_t id = 0; remainder > 0; id++) {
		auto size = static_cast<streamsize>(min(Chunk::SIZE, remainder));
		inFile.read(bytes, size);

		Chunk chunk { m_fileId, id, false };

		auto decompressedName = DecompressedName(chunk);
		auto compressedName = CompressedName(chunk);

		FileSystem::WriteFile(decompressedName, bytes, size);

		if (PackManager::Compress(decompressedName, compressedName)) {
			FileSystem::RemoveFile(decompressedName);
			chunk.Compressed(true);
		}

		auto buffer2 = FileSystem::ReadFile(compressedName);

		Encryption encryption;
		int res = encryption.SetKey(encryptionPassword);
		if (res != 0) { return false; }
		if (!encryption.IsReady()) { return false; }

		vector<uint8_t> data;

		try
		{
			data = stov(buffer2->Bytes());
			size = encryption.Encrypt(data, (int)buffer2->Size());
			if (size < 0) { return false; }
		}
		catch (...)
		{
			return false;
		}

		FileSystem::WriteFile(compressedName, vtos(data).c_str(), size);

		m_chunks.push_back(chunk);

		remainder -= size;
	}

	inFile.close();

	return true;
}

bool Unit::RemoveNotSplit()
{
	auto filePath = FileSystem::Combine(FileSystem::GetCloudFolder(), m_fileName);
	return FileSystem::RemoveFile(filePath);
}

bool Unit::Send()
{
	for (int c = 0; c < m_chunks.size(); c += 2) {
		if (!m_chunks[c].Local()) { continue; }

		auto fileName = ChunkName(m_chunks[c]);

		std::shared_ptr<Buffer> buffer = GetChunk(fileName);
		string white = m_security->GetNextWhite();

		bool sent1_command = m_netManager->Send(white, UploadCommand(m_chunks[c].Id()));
		bool sent1 = sent1_command ? m_netManager->Send(white, buffer) : false;

		white = m_security->GetNextWhite();
		bool sent2_command = m_netManager->Send(white, UploadCommand(m_chunks[c].Id()));
		bool sent2 = sent2_command ? m_netManager->Send(white, buffer) : false;

		if (sent1 || sent2) {
			FileSystem::RemoveFile(fileName);
			m_chunks[c].Local(false);
		}
	}

	return false;
}

string Unit::UploadCommand(uint32_t id)
{
	stringstream ss;
	ss << "upload" << " ";
	ss << m_fileId.ToString() << " ";
	ss << id;
	return ss.str();
}

bool Unit::Restore()
{
	switch (m_tier.TierLevel()) {
	case 0: break;
	case 2:
		if (!Gather()) { return false; }
		[[fallthrough]];
	case 1:
		if (!Decompress()) { return false; }
		break;
	}

	m_tier.Set(0);

	return true;
}

bool Unit::Gather()
{
	string firstWhite = m_security->GetNextWhite();
	string white = firstWhite;

	for (const auto & chunk : m_chunks)
	{
		if (chunk.Local()) { continue; }

		bool received = false;
		while (!received)
		{
			/*bool sent =*/ m_netManager->Send(white, DownloadCommand(chunk.Id()));
			received = m_netManager->Download(white, ChunkName(chunk).generic_string());
			if (!received)
			{
				white = m_security->GetNextWhite(); // »щем по следующему адресу.
				if (white == firstWhite)
				{
					return false; // нигде не нашли - выходим
				}
			}
		}
	}

	return true;
}

string Unit::DownloadCommand(uint32_t id)
{
	stringstream ss;
	ss << "download" << " ";
	ss << m_fileId.ToString() << " ";
	ss << id;
	return ss.str();
}

bool Unit::Decompress()
{
	fs::path filePath = FileSystem::Combine(FileSystem::GetCloudFolder(), m_fileName);
	fs::path dcFilePath = FileSystem::Combine(FileSystem::GetCloudFolder(), m_fileName + "_");

	if (FileSystem::Exists(filePath)) { return true; }

	ofstream outFile;
	outFile.open(dcFilePath, ios::binary);
	if (outFile.fail()) { return false; }

	for (const auto & chunk : m_chunks) {
		if (chunk.Compressed()) {
			auto compressedName = CompressedName(chunk);
			auto decompressedName = DecompressedName(chunk);

			if (PackManager::Decompress(compressedName, decompressedName)) {

				auto buffer = FileSystem::ReadFile(decompressedName);
				int size = (int)buffer->Size();

				Encryption encryption;
				int res = encryption.SetKey(encryptionPassword);
				if (res != 0) { return false; }
				if (!encryption.IsReady()) { return false; }

				std::vector<uint8_t> data;

				try
				{
					data = stov(buffer->Bytes());
					size = encryption.Decrypt(data, (int)buffer->Size());
					if (size < 0) { return false; }
				}
				catch (...)
				{
					return false;
				}

				outFile.write(vtos(data).c_str(), size);

				FileSystem::RemoveFile(decompressedName);
			} else {
				return false;
			}
		} else {
			auto decompressedName = DecompressedName(chunk);

			auto buffer = FileSystem::ReadFile(decompressedName);
			outFile.write(const_cast<char *>(buffer->Bytes()), buffer->Size());
		}
	}

	outFile.close();

	FileSystem::Rename(dcFilePath, filePath);

	return true;
}

fs::path Unit::ChunkName(const Chunk & chunk)
{
	if (chunk.Compressed()) {
		return CompressedName(chunk);
	} else {
		return DecompressedName(chunk);
	}
}

fs::path Unit::CompressedName(const Chunk & chunk)
{
	fs::path filePath = FileSystem::GetMetaFolder();
	filePath /= chunk.GetFileId().ToString();
	filePath /= to_string(chunk.Id());
	filePath /= "_c";
	return filePath;
}

fs::path Unit::DecompressedName(const Chunk & chunk)
{
	fs::path filePath = FileSystem::GetMetaFolder();
	filePath /= chunk.GetFileId().ToString();
	filePath /= to_string(chunk.Id());
	return filePath;
}

std::shared_ptr<Buffer> Unit::GetChunk(const fs::path & fileName)
{
	streamsize size = FileSystem::FileSize(fileName);
	auto buffer = std::make_shared<Buffer>(static_cast<size_t>(size));

	ifstream dcFile;
	dcFile.open(fileName, ios::binary);
	dcFile.read(const_cast<char *>(buffer->Bytes()), size);
	dcFile.close();

	return buffer;
}
