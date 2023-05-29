#include "Settings.h"

#include "../Files/FileSystem.h"

void Settings::Load()
{
	auto settingsPath = GetSettingsPath();

	if (!FileSystem::Exists(settingsPath))
	{ return; }

    auto buffer = FileSystem::ReadFile(settingsPath);

    Document jsonDocument;
    jsonDocument.Parse(buffer->Bytes(), buffer->Size());

    Value & cloudFolderValue = jsonDocument["CloudFolder"];
    m_cloudFolder = cloudFolderValue.GetString();
}

void Settings::Save()
{
	auto settingsPath = GetSettingsPath();

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String("CloudFolder");
    writer.String(m_cloudFolder.c_str(), static_cast<SizeType>(m_cloudFolder.length()));

    writer.EndObject();

    FileSystem::WriteFile(settingsPath, sb.GetString(), sb.GetSize());
}

fs::path Settings::GetSettingsPath()
{
	fs::path settingsPath = FileSystem::GetMetaFolder();
	settingsPath /= "settings.json";

	return settingsPath;
}
