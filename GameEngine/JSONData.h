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

#include <string>
#include "JSONDefinitions.h"

class Vector2;
class Vector3;
class Vector4;
class Matrix;
struct Color;

class JSONData
{
public:
	explicit JSONData(const JSONDoc::ConstMemberIterator& data);
	JSONData(const JSONDoc* document, const std::string& memberName);

	// IsValid: returns the object's valid status checked at creation
	bool IsValid(void) const;

	// IsValid: compares iterator against object data's memberName array's end, returns false if equal
	bool IsValidIterator(const JSONDoc::ConstValueIterator& iter, const std::string& memberName) const;

	// GetName: returns a std::string containing JSON member name
	std::string GetName(void) const;

	// HasMember: returns true if data contains member with name
	bool HasMember(const std::string& name) const;

	// FindMember: if found returns JSONData object pointing to JSON member of name
	JSONData FindMember(const std::string& name) const;

	// GetArrayBegin: returns a JSON value iterator poiting to the first member in array
	JSONDoc::ConstValueIterator GetArrayBegin(void) const;

	// GetArrayEnd: returns a JSON value iterator poting to the end of the array
	JSONDoc::ConstValueIterator GetArrayEnd(void) const;

	// GetArrayValueIterator: returns a JSON value iterator pointing to the first member in memberName member array
	JSONDoc::ConstValueIterator GetArrayValueIterator(const std::string& memberName) const;

	// Data getters: write data to value references, return true if successful
	bool GetString(const std::string& attribute, std::string& value) const;
	bool GetBool(const std::string& attribute, bool& value) const;
	bool GetInt32(const std::string& attribute, int32_t& value) const;
	bool GetInt64(const std::string& attribute, int64_t& value) const;
	bool GetUInt32(const std::string& attribute, uint32_t& value) const;
	bool GetUInt64(const std::string& attribute, uint64_t& value) const;
	bool GetFloat(const std::string& attribute, float& value) const;
	bool GetDouble(const std::string& attribute, double& value) const;
	bool GetVector2(const std::string& attribute, Vector2& value) const;
	bool GetVector3(const std::string& attribute, Vector3& value) const;
	bool GetVector4(const std::string& attribute, Vector4& value) const;
	bool GetColor(const std::string& attribute, Color& value) const;
	bool GetMatrix(const std::string& attribute, Matrix& value) const;

private:
	JSONDoc::ConstMemberIterator m_data;
	bool m_isValid;
};
