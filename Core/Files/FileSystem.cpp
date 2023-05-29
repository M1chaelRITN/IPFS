#include "FileSystem.h"

#include "../Pack/PackManager.h"

#ifdef _MSC_VER
#  include <shlobj.h>
#endif

#include <cstdlib>

#include <fstream>
#include <utility>
#include <exception>
using namespace std;

fs::path FileSystem::GetUserFolder()
{
	return GetEnv("HOME");
}

fs::path FileSystem::GetAppDataLocalFolder()
{
#ifdef _MSC_VER
	fs::path appData = GetEnv("APPDATA");
	//appData /= "Local";
#else
	fs::path appData = GetEnv("HOME");
	appData /= ".config";
#endif
	return appData;
}

fs::path FileSystem::GetCloudFolder()
{
	fs::path path = GetUserFolder();

	path /= "NewInternet";

	if (!fs::exists(path))
	{
		fs::create_directory(path);
	}

	return path;
}

fs::path FileSystem::GetMetaFolder()
{
	fs::path path = GetAppDataLocalFolder();

	path /= "NewInternet";

	if (!fs::exists(path))
	{
		fs::create_directory(path);
	}

	return path;
}

bool FileSystem::Exists(const fs::path & filePath)
{
	return fs::exists(filePath);
}

fs::path FileSystem::Combine(const fs::path & root, const fs::path & filePath, bool createFolders /*= false*/)
{
	fs::path out = root;
	std::string filePathStr (filePath.generic_string());

	size_t pos = 0;
	while (pos != string::npos)
	{
		size_t slash = filePathStr.find_first_of("/\\", pos);
		if (slash == string::npos)
		{
			out /= filePathStr.substr(pos);
			pos = slash;
		}
		else
		{
			out /= filePathStr.substr(pos, slash - pos);
			pos = slash + 1;

			if (createFolders && !fs::exists(out))
			{
				fs::create_directory(out);
			}
		}
	}

	return out;
}

std::shared_ptr<Buffer> FileSystem::ReadFile(const fs::path & filePath)
{
	ifstream file;
	size_t fileSize = FileSize(filePath);
	auto buffer = make_shared<Buffer>(fileSize);

	file.open(filePath, ios::binary);
	if (file.fail())
	{
		throw runtime_error("File does not exist");
	}
	else
	{
		file.read((char *)(buffer->Bytes()), static_cast<streamsize>(fileSize));
		file.close();
	}

	return buffer;
}

streampos FileSystem::FileSize(const fs::path & filePath)
{
	streampos fsize = 0;
	ifstream file(filePath, ios::binary);

	fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;
	file.close();

	return fsize;
}

void FileSystem::WriteFile(const fs::path & filePath, const char * content, size_t size)
{
	ofstream file;
	file.open(filePath, ios::binary);
	if (file.is_open())
	{
		file.write(content, static_cast<streamsize>(size));
	}
	else
	{
		throw runtime_error("Can't open file");
	}
	file.close();
}

void FileSystem::WriteFile(const fs::path & filePath, std::shared_ptr<Buffer> buffer)
{
	ofstream file;
	file.open(filePath, ios::binary);
	if (file.is_open())
	{
		file.write((char *)(buffer->Bytes()), buffer->Size());
	}
	else
	{
		throw runtime_error("Can't open file");
	}
	file.close();
}

bool FileSystem::RemoveFile(const fs::path & filePath)
{
	return fs::remove(filePath);
}

bool FileSystem::Copy(const fs::path & fromFile, const fs::path & toFile)
{
	return fs::copy_file(fromFile, toFile, fs::copy_options::overwrite_existing);
}

bool FileSystem::Rename(const fs::path & fromFile, const fs::path & toFile)
{
	fs::rename(fromFile, toFile);
	return true;
}

std::string FileSystem::GetEnv(const std::string & varName)
{
#ifdef _MSC_VER
	char path[MAX_PATH];

	if (varName == "HOME")
	{
		if (SHGetFolderPath(nullptr, CSIDL_PROFILE, nullptr, 0, path) == 0)
		{
			return path;
		}
	}

	if (varName == "APPDATA")
	{
		if (SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, path) == 0)
		{
			return path;
		}
	}

	return "";

#else
	return getenv("HOME");
#endif
}

