#pragma once

#include <string>
#include <vector>

class Parameters
{
public:
	Parameters(int argc, char ** argv);

	bool Failed();
	int Size() const { return m_size; }
	std::string GetUsage() const { return m_usage; }

	std::string Method() const { return m_method; }
	std::string RemoteIp() const { return m_remote; }
	std::string FileName() const { return m_fileName; }
	std::string Directory() const { return m_directory; }

private:
	int m_size;
	const std::string m_usage = "usage: ni add|remove|list|get -f <filename> -d <directory>";

	std::string m_method;
	std::string m_remote;
	std::string m_fileName;
	std::string m_directory;

	std::string Unquote(const std::string & parameter);
	//std::wstring s2ws(const std::string & str);

};
