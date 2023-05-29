#pragma once

#include "../Parameters/Parameters.h"
#include "Unit.h"

#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

class Repository
{
public:
	Repository();

	void Run(const Parameters & parameters);
	
	bool Add(const std::string & fileName, const std::string & directory);
	bool Remove(const std::string & fileName);

	[[nodiscard]]
	std::vector<Unit> List(const std::string & path = "") const;
	std::string Get(const std::string & fileName);

private:
	std::vector<Unit> m_files;
	std::shared_ptr<INetManager> m_netManager;
	std::shared_ptr<Security> m_security;

	std::vector<Unit> LoadFiles(const fs::path & cloudPath);

};
