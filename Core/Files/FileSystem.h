#pragma once

#include "Buffer.h"

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

class FileSystem
{
public:
	static fs::path GetCloudFolder();
	static fs::path GetMetaFolder();

	static bool Exists(const fs::path & filePath);
	static fs::path Combine(const fs::path & root, const fs::path & filePath, bool createFolders = false);

	static std::shared_ptr<Buffer> ReadFile(const fs::path & filePath);
	static std::streampos FileSize(const fs::path & filePath);
	static void WriteFile(const fs::path & filePath, const char * content, size_t size);
	static void WriteFile(const fs::path & filePath, std::shared_ptr<Buffer> buffer);
	static bool RemoveFile(const fs::path & filePath);
	static bool Copy(const fs::path & fromFile, const fs::path & toFile);
	static bool Rename(const fs::path & fromFile, const fs::path & toFile);

private:
	static fs::path GetUserFolder();
	static fs::path GetAppDataLocalFolder();

	static std::string GetEnv(const std::string & var);

};
