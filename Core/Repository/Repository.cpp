#include "Repository.h"

#include "../Files/FileSystem.h"
#include "../Pack/PackManager.h"

#include <iostream>
#include <algorithm>
#include <exception>
using namespace std;

Repository::Repository()
{
	auto cloudFolder = FileSystem::GetCloudFolder();
	m_files = LoadFiles(cloudFolder);
	m_netManager = make_shared<NetManager>();
	m_security = make_shared<Security>(m_netManager);
}

vector<Unit> Repository::LoadFiles(const fs::path & cloudPath)
{
	vector<Unit> files;

	for (const auto & entry : fs::directory_iterator(cloudPath))
	{
		if (entry.is_directory())
		{
			auto children = LoadFiles(entry.path());
			for (const auto & child : children)
			{
				files.push_back(child);
			}
		}
		else
		{
			files.emplace_back(entry.path().generic_string(), entry.file_size(), m_netManager, m_security);
		}
	}

	return files;
}

void Repository::Run(const Parameters & parameters)
{
	if (parameters.Method() == "add")
	{
		if (!Add(parameters.FileName(), parameters.Directory()))
		{
			throw runtime_error("Add file error");
		}
	}
	else if (parameters.Method() == "remove")
	{
		if (!Remove(parameters.FileName()))
		{
			throw runtime_error("Remove file error");
		}
	}
	else if (parameters.Method() == "list")
	{
		string path = parameters.Directory().empty() ? parameters.FileName() : parameters.Directory();
		auto files = List(path);
		for (const auto & file : files)
		{
			cout << file.FileName() << endl;
		}
	}
	else if (parameters.Method() == "get")
	{
		auto filePath = Get(parameters.FileName());
		if (!filePath.empty())
		{
			cout << filePath << endl;
		}
	}
}

bool Repository::Add(const string & fileName, const string & directory)
{
	fs::path fromFile = fileName;
	auto fromFileName = fromFile.filename();

	auto toFile = FileSystem::Combine(FileSystem::GetCloudFolder(), directory, true);
	toFile /= fromFileName;

	bool copied = FileSystem::Copy(fromFile, toFile);

	auto cloudPath = FileSystem::Combine(directory, fromFileName);

	m_files.emplace_back(cloudPath.generic_string(), FileSystem::FileSize(toFile), m_netManager, m_security);
	m_files[m_files.size() - 1].Modified();

	return copied;
}

bool Repository::Remove(const string & filePath)
{
	auto cloudPath = FileSystem::Combine(FileSystem::GetCloudFolder(), filePath);
	auto metaPath = FileSystem::Combine(FileSystem::GetMetaFolder(), filePath);

	bool removed = FileSystem::RemoveFile(cloudPath);
	FileSystem::RemoveFile(metaPath);

	const auto new_end(remove_if(begin(m_files), end(m_files), [&filePath](const Unit & unit) {
		return filePath == unit.FileName();
	}));
	m_files.erase(new_end, end(m_files));

	return removed;
}

std::vector<Unit> Repository::List(const std::string & path /*= ""*/) const
{
	if (path.empty()) { return m_files; }

	std::vector<Unit> files;
	copy_if(m_files.begin(), m_files.end(), back_inserter(files), [&path](const Unit & unit) {
		return unit.FileName().substr(0, min(unit.FileName().length(), path.length())) == path;
	});

	return files;
}

std::string Repository::Get(const std::string & fileName)
{
	auto filePath = FileSystem::Combine(FileSystem::GetCloudFolder(), fileName);

	auto found = find_if(m_files.begin(), m_files.end(), [&fileName](const Unit & unit) {
		return unit.FileName().substr(0, min(unit.FileName().length(), fileName.length())) == fileName;
	});

	if (found == m_files.end())
	{
		return "";
	}
	else
	{
		if (!found->Restore()) { return ""; }
	}

	return filePath.generic_string();
}
