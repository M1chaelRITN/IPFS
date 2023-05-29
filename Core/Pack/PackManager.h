#pragma once

#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class PackManager
{
public:
	static bool Compress(const fs::path & inFile, const fs::path & outFile);
	static bool Decompress(const fs::path & inFile, const fs::path & outFile);

};
