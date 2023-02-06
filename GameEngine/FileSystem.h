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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <filesystem>
#include <string>
#include <vector>
#include <functional>
#include "ISystem.h"

namespace fs = std::filesystem;

class DirectoryObject
{
public:
	std::string m_path; // stores full path to the object beginning from root
	std::string m_filename; // stores file/folder name including extension
	std::string m_name; // stores file/folder name without extension
	std::string m_extension; // stores file extension, empty string if folder or file has no extension
	bool isDirectory = false;
};

class FileSystem : public ISystem
{
public:
	FileSystem(App* app, GameObjectFactory* GOF);
	~FileSystem(void);

	bool Initialize(INIReader* ini) override final;
	void Update(float deltaTime) override final;
	void Exit(void) override final;
	
	/* Get a list of files of a particular type: e.g. ".txt", ".json" */
	void GetFiles(const char* type, std::vector<const DirectoryObject*>& list) const;

	void RegisterMessages(void);
	void ProcessMessage(const Message* message) override final;

	static std::string NormalizePath(const std::string& path); // normalizes path to use universal Unix style

private:
	void FulfillFileRequests(void); // checks for any pending file requests and fulfills them

	void ScanFolder(const std::string& path);
	void ScanFolder_Internal(fs::path path, std::vector<DirectoryObject>& objects);
	bool TypeMatch_Internal(const char* type, const char* file) const;

private:
	std::vector<DirectoryObject> m_objects;

	// Stores loader functions from requesting systems for all requested file types
	std::map<std::string, std::vector<std::function<void(const std::string&)>>> m_fileRequests;
};

#endif
