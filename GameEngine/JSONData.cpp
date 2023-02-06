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

#include "JSONData.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Color.h"

JSONData::JSONData(const JSONDoc::ConstMemberIterator& data)
	: m_data(data)
	, m_isValid(true)
{}

JSONData::JSONData(const JSONDoc* document, const std::string& memberName)
	: m_isValid(true)
{
	m_data = document->FindMember(memberName.c_str());
	if (m_data == document->MemberEnd())
	{
		m_isValid = false;
	}
}

bool JSONData::IsValid(void) const
{
	return m_isValid;
}

bool JSONData::IsValidIterator(const JSONDoc::ConstValueIterator& iter, const std::string& memberName) const
{
	if (m_data->value.HasMember(memberName.c_str()))
	{
		if (iter != m_data->value[memberName.c_str()].GetArray().End())
		{
			return true;
		}
	}
	return false;
}

std::string JSONData::GetName(void) const
{
	return m_data->name.GetString();
}

bool JSONData::HasMember(const std::string& name) const
{
	if (m_data->value.HasMember(name.c_str()))
	{
		return true;
	}
	return false;
}

JSONData JSONData::FindMember(const std::string& name) const
{
	JSONData foundMember = JSONData(m_data->value.FindMember(name.c_str()));
	if (foundMember.m_data == m_data->value.MemberEnd())
	{
		foundMember.m_isValid = false;
	}
	return foundMember;
}

JSONDoc::ConstValueIterator JSONData::GetArrayBegin(void) const
{
	return m_data->value.GetArray().Begin();
}

JSONDoc::ConstValueIterator JSONData::GetArrayEnd(void) const
{
	return m_data->value.GetArray().End();
}

JSONDoc::ConstValueIterator JSONData::GetArrayValueIterator(const std::string& memberName) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(memberName.c_str());
	if (member != m_data->value.MemberEnd())
	{
		return member->value.GetArray().Begin();
	}
	return member->value.End();
}

bool JSONData::GetString(const std::string& attribute, std::string& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = member->value.GetString();
		return true;
	}
	return false;
}

bool JSONData::GetBool(const std::string& attribute, bool& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = member->value.GetBool();
		return true;
	}
	return false;
}

bool JSONData::GetInt32(const std::string& attribute, int32_t& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = static_cast<int32_t>(member->value.GetInt64());
		return true;
	}
	return false;
}

bool JSONData::GetInt64(const std::string& attribute, int64_t& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = member->value.GetInt64();
		return true;
	}
	return false;
}

bool JSONData::GetUInt32(const std::string& attribute, uint32_t& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = static_cast<uint32_t>(member->value.GetUint64());
		return true;
	}
	return false;
}

bool JSONData::GetUInt64(const std::string& attribute, uint64_t& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = member->value.GetUint64();
		return true;
	}
	return false;
}

bool JSONData::GetFloat(const std::string& attribute, float& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = member->value.GetFloat();
		return true;
	}
	return false;
}

bool JSONData::GetDouble(const std::string& attribute, double& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		value = member->value.GetDouble();
		return true;
	}
	return false;
}

bool JSONData::GetVector2(const std::string& attribute, Vector2& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		if (member->value.HasMember("x"))
		{
			value.x = member->value["x"].GetFloat();
		}
		if (member->value.HasMember("y"))
		{
			value.y = member->value["y"].GetFloat();
		}
		return true;
	}
	return false;
}

bool JSONData::GetVector3(const std::string& attribute, Vector3& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		if (member->value.HasMember("x"))
		{
			value.x = member->value["x"].GetFloat();
		}
		if (member->value.HasMember("y"))
		{
			value.y = member->value["y"].GetFloat();
		}
		if (member->value.HasMember("z"))
		{
			value.z = member->value["z"].GetFloat();
		}
		return true;
	}
	return false;
}

bool JSONData::GetVector4(const std::string& attribute, Vector4& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		if (member->value.HasMember("x"))
		{
			value.x = member->value["x"].GetFloat();
		}
		if (member->value.HasMember("y"))
		{
			value.y = member->value["y"].GetFloat();
		}
		if (member->value.HasMember("z"))
		{
			value.z = member->value["z"].GetFloat();
		}
		if (member->value.HasMember("w"))
		{
			value.w = member->value["w"].GetFloat();
		}
		return true;
	}
	return false;
}

bool JSONData::GetColor(const std::string& attribute, Color& value) const
{
	Vector4 asFloat{};
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		if (member->value.HasMember("R"))
		{
			asFloat.x = member->value["R"].GetFloat();
		}
		if (member->value.HasMember("G"))
		{
			asFloat.y = member->value["G"].GetFloat();
		}
		if (member->value.HasMember("B"))
		{
			asFloat.z = member->value["B"].GetFloat();
		}
		if (member->value.HasMember("A"))
		{
			asFloat.w = member->value["A"].GetFloat();
		}

		value = Color(asFloat);

		return true;
	}
	return false;
}

bool JSONData::GetMatrix(const std::string& attribute, Matrix& value) const
{
	JSONDoc::ConstMemberIterator member = m_data->value.FindMember(attribute.c_str());
	if (member != m_data->value.MemberEnd())
	{
		JSONDoc::ConstMemberIterator matrixIter = member->value.MemberBegin();
		float* grid = &value.grid[0][0];
		for (int i = 0; i < 16 && matrixIter != member->value.MemberEnd(); ++i, ++matrixIter)
		{
			*(grid + i) = matrixIter->value.GetFloat();
		}
		return true;
	}
	return false;
}
