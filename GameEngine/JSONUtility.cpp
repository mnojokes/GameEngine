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

#include "JSONUtility.h"

JSONDoc* JSONUtility::ParseFile(const std::string& path)
{
	FILE* fp = fopen(path.c_str(), "r");
	if (fp == nullptr)
	{
		fprintf(stderr, "JSONParser::%s: failed to open file %s\n", __func__, path.c_str());
		return nullptr;
	}

	/* Find out JSON file size */
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);

	/* Allocate buffer and read */
	char* buffer = static_cast<char*>(malloc(sizeof(char) * fileSize));
	if (buffer == nullptr)
	{
		fprintf(stderr, "JSONParser::%s: failed to allocate buffer\n", __func__);
		return nullptr;
	}

	json::FileReadStream is(fp, buffer, fileSize);
	JSONDoc* newDoc = new JSONDoc;
	newDoc->ParseStream(is);

	fclose(fp);
	free(buffer);

	return newDoc;
}
