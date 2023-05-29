#include "../Core/framework.h"

#include "MockNetManager.h"
#include "WrongMockNetManager.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <memory>
using namespace std;
using namespace chrono;
using namespace chrono_literals;

TEST_CASE("NetworkTestCase")
{
	SECTION("White list test")
	{
		Security security { make_shared<MockNetManager>() };

		auto hostIp = security.CheckOther("127.0.0.1");

		REQUIRE(hostIp == HostIp::WhiteList);
	}

	SECTION("Black list test")
	{
		Security security { make_shared<WrongMockNetManager>() };

		auto hostIp = security.CheckOther("127.0.0.1");

		REQUIRE(hostIp == HostIp::BlackList);
	}
}

TEST_CASE("RepositoryTestCase", "RepositoryTest")
{
	SECTION("RepositoryTest")
	{
		string file = "file.txt";
		string dir = "path/to/cloud";
		string filePath = dir + "/" + file;

		Repository repository;
		repository.Add(file, dir);
		vector<Unit> files = repository.List();
		bool fileExitsts = find_if(files.begin(), files.end(), [&filePath](const Unit & unit) {
			return filePath == unit.FileName();
		}) != files.end();

		REQUIRE(fileExitsts);

		auto fp = FileSystem::GetCloudFolder();
		fp /= dir;
		fp /= file;
		bool diskFileExitsts = FileSystem::Exists(fp);

		REQUIRE(diskFileExitsts);

		repository.Remove(filePath);
	}

	SECTION("TierTest")
	{
		string file = "file.txt";
		string dir = "path/to/cloud";
		string filePath = dir + "/" + file;

		Repository repository;
		repository.Add(file, dir);
		vector<Unit> files = repository.List();

		REQUIRE(files.size() == 1);

		Unit f = files[0];
		Tier::CHANGE_TIER_SEC = 1;
		this_thread::sleep_for(1000ms);
		f.CheckTier();
		int tier = f.TierLevel();

		REQUIRE(tier == 1);

		repository.Remove(filePath);
	}

	SECTION("EncryptionTest")
	{
		string file = "file.txt";
		string dir = "path/to/cloud";
		string filePath = dir + "/" + file;

		Repository repository;
		repository.Add(file, dir);
		vector<Unit> files = repository.List();

		REQUIRE(files.size() == 1);

		Unit f = files[0];
		Tier::CHANGE_TIER_SEC = 1;
		this_thread::sleep_for(1000ms);
		f.CheckTier();

		bool restored = f.Restore();
		REQUIRE(restored);

		//auto fp = FileSystem::GetCloudFolder();
		//fp /= dir;
		//fp /= file;

		repository.Remove(filePath);
	}
}

TEST_CASE("SettingsTestCase", "SettingsTest")
{
	string cloudFolder = "Cloud";
	string loadedCloudFolder;

	{
		Settings settings1;
		settings1.CloudFolder(cloudFolder);
		settings1.Save();
	}

	{
		Settings settings2;
		settings2.Load();
		loadedCloudFolder = settings2.CloudFolder();
	}

	REQUIRE(cloudFolder == loadedCloudFolder);
}
