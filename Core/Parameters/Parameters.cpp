#include "Parameters.h"

Parameters::Parameters(int argc, char ** argv) : m_size(argc)
{
	for (int p = 1; p < m_size; p++)
	{
		std::string parameter = argv[p];
		if (p == 1)
		{
			m_method = parameter;
		}
		else
		{
			if (parameter == "-r")
			{
				if (p + 1 == m_size)
				{
					m_method = "";
					return;
				}
				m_remote = Unquote(argv[++p]);
			}
			if (parameter == "-f")
			{
				if (p + 1 == m_size)
				{
					m_method = "";
					return;
				}
				m_fileName = /*s2ws(*/Unquote(argv[++p])/*)*/;
			}
			if (parameter == "-d")
			{
				if (p + 1 == m_size)
				{
					m_method = "";
					return;
				}
				m_directory = /*s2ws(*/Unquote(argv[++p])/*)*/;
			}
		}
	}
}

bool Parameters::Failed()
{
	if (m_size < 2 || m_method.empty())
	{
		return true;
	}

	if ((m_method == "upload" || m_method == "download") &&
		(m_remote.empty() || m_fileName.empty()))
	{
		return true;
	}

	if (m_method == "add" && (m_fileName.empty() || m_directory.empty()))
	{
		return true;
	}

	if (m_method == "remove" && m_fileName.empty())
	{
		return true;
	}

	if (m_method == "get" && m_fileName.empty())
	{
		return true;
	}

	return false;
}

std::string Parameters::Unquote(const std::string & parameter)
{
	if (parameter.substr(0, 1) == "\"")
	{
		return parameter.substr(1, parameter.length() - 2);
	}

	return parameter;
}
//
//std::wstring Parameters::s2ws(const std::string & str)
//{
//	return std::wstring(str.begin(), str.end());
//}
