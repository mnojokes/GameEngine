// Copyright (C) 2023  Mantas Naujokas
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "FileSystem.h"
#include "App.h"
#include "Message.h"
#include "MessagesGraphicsSystem.h"
#include "MessagesWorldManager.h"
#include "MessageFileRequest.h"
#include "StringUtility.h"

FileSystem::FileSystem(App* app, GameObjectFactory* GOF)
	: ISystem(app, GOF)
{}

FileSystem::~FileSystem(void)
{}

bool FileSystem::Initialize(INIReader* ini)
{
	m_iniReader = ini;
	
	const std::string& assetPath = m_appPtr->GetAssetPath();

	ScanFolder(assetPath);

	return true;
}

void FileSystem::Update(float deltaTime)
{
	FulfillFileRequests();
}

void FileSystem::Exit(void)
{
	m_objects.clear();
}

void FileSystem::ScanFolder(const std::string& path)
{
	fs::path dirPath(path);
	
	if (!fs::is_directory(path))
	{
		return;
	}

	DirectoryObject root;
	root.m_path = NormalizePath(path);
	root.isDirectory = true;
	m_objects.push_back(root);

	ScanFolder_Internal(dirPath, m_objects);
}

void FileSystem::GetFiles(const char* type, std::vector<const DirectoryObject*>& list) const
{
	for (size_t i = 0U; i < m_objects.size(); ++i)
	{
		if (TypeMatch_Internal(type, m_objects[i].m_path.c_str()) && !m_objects[i].isDirectory)
		{
			list.push_back(&m_objects[i]);
		}
	}	
}

void FileSystem::RegisterMessages(void)
{
	m_messenger.RegisterMessage<FileRequestMessage>(this);
}

void FileSystem::ProcessMessage(const Message* message)
{
	if (message->IsType<FileRequestMessage>())
	{
		const FileRequestMessage* msg = static_cast<const FileRequestMessage*>(message);

		auto extensionIter = m_fileRequests.find(msg->m_extension);
		if (extensionIter != m_fileRequests.end())
		{
			extensionIter->second.push_back(msg->m_FileLoader);
		}
		else
		{
			std::vector<std::function<void(const std::string&)>> loaders;
			loaders.push_back(msg->m_FileLoader);
			m_fileRequests.insert({ msg->m_extension, loaders });
		}
	}
}

void FileSystem::FulfillFileRequests(void)
{
	if (!m_fileRequests.empty())
	{
		for (auto fileIter : m_objects)
		{
			if (!fileIter.isDirectory)
			{
				auto extensionIter = m_fileRequests.find(fileIter.m_extension);
				if (extensionIter != m_fileRequests.end())
				{
					for (auto requestIter : extensionIter->second)
					{
						requestIter(fileIter.m_path);
					}
				}
			}
		}

		m_fileRequests.clear();
	}
}

void FileSystem::ScanFolder_Internal(fs::path path, std::vector<DirectoryObject>& objects)
{
	DirectoryObject dirObj;

	for (fs::directory_entry entry : fs::directory_iterator(path))
	{
		dirObj.m_path = NormalizePath(entry.path().string());
		size_t index = dirObj.m_path.find_last_of('/');
		dirObj.m_name = dirObj.m_filename = dirObj.m_path.substr(index + 1U);
		if (fs::is_directory(entry.path()))
		{
			dirObj.isDirectory = true;
			ScanFolder_Internal(entry.path(), objects);
		}
		else if (fs::is_regular_file(entry.path()))
		{
			dirObj.m_name = StringUtility::RemoveFileExtension(dirObj.m_name);
			dirObj.m_extension = StringUtility::ExtensionFromName(dirObj.m_filename);
			dirObj.isDirectory = false;
		}

		objects.push_back(dirObj);
	}
}

bool FileSystem::TypeMatch_Internal(const char* type, const char* file) const
{
	size_t typeLen = strlen(type);
	size_t fileLen = strlen(file);

	const char* suffix = file + (fileLen - typeLen);

	for (size_t i = 0; i < typeLen; ++i)
	{
		if (type[i] != suffix[i])
		{
			return false;
		}
	}

	return true;
}

std::string FileSystem::NormalizePath(const std::string& path)
{
	std::string newStr = path;
	size_t pos = 0;
	while ((pos = newStr.find("\\", pos)) != std::string::npos)
	{
		newStr.replace(pos, 1, "/");
	}

	return newStr;
}
