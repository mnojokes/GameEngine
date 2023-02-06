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

#include "StringUtility.h"

uint64_t StringUtility::GetIndexFromName(const std::string& name)
{
	size_t indexAt = FindIndexPos(name);
	if (indexAt == std::string::npos)
	{
		fprintf(stderr, "GameObjectFactory::%s: \"%s\" does not contain a valid index, returned 0\n", __func__, name.c_str());
		return 0U;
	}

	std::string indexStr = name.substr(indexAt + 1);
	uint64_t index = std::atoll(indexStr.c_str());
	return index;
}

std::string StringUtility::RemoveIndexFromName(const std::string& name)
{
	std::string nameCopy = name;
	size_t indexAt = FindIndexPos(nameCopy);
	if (indexAt != std::string::npos)
	{
		nameCopy.erase(nameCopy.begin() + indexAt, nameCopy.end());
	}
	return nameCopy;
}

std::string StringUtility::RemoveFileExtension(const std::string& filename)
{
	std::string filenameCopy = filename;
	size_t extensionAt = filenameCopy.find_last_of('.');
	if (extensionAt != std::string::npos)
	{
		filenameCopy.erase(filenameCopy.begin() + extensionAt, filenameCopy.end());
	}
	return filenameCopy;
}

std::string StringUtility::ExtensionFromName(const std::string& filename)
{
	size_t index = filename.find_last_of('.');
	if (index != std::string::npos)
	{
		return filename.substr(index);
	}
	return std::string();
}

size_t StringUtility::FindIndexPos(const std::string& str)
{
	return str.find_last_of('_'); // position before first number
}

std::string StringUtility::ExtractFilenameFromPath(const std::string& path)
{
	size_t index = path.find_last_of('/');
	if (index != std::string::npos)
	{
		return path.substr(index + 1);
	}
	return path;
}
