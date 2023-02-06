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

#ifndef STRINGUTILITY_H
#define STRINGUTILITY_H

#include <string>

class StringUtility
{
public:
	// Finds index in name string. Accepted format: "name_index", returns index or 0 if nothing found
	static uint64_t GetIndexFromName(const std::string& name);
	
	// Finds index in name string, returns a copy of name without index. Accepted format: "name_index"
	static std::string RemoveIndexFromName(const std::string& name);

	// Finds the extension in the filename, returns a string with the filename without extension. Accepted format "name.ext"
	static std::string RemoveFileExtension(const std::string& filename);

	// Finds the extension in the filename, returns a string with the extension, empty string if no extension found. Accepted format "name.ext"
	static std::string ExtensionFromName(const std::string& filename);

	// Extracts file name from file path with extension
	static std::string ExtractFilenameFromPath(const std::string& path);

private:
	static size_t FindIndexPos(const std::string& str);
};


#endif
