#pragma once

#include <string>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

class Settings
{
public:
	void Load();
	void Save();

	void CloudFolder(const std::string & cloudFolder) { m_cloudFolder = cloudFolder; }
	[[nodiscard]]
	std::string CloudFolder() const { return m_cloudFolder; }

private:
	std::string m_cloudFolder;

	fs::path GetSettingsPath();

};
