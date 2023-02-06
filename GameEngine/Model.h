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

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include "GraphicsDefines.h"
#include "DirectXUtil.h"

struct ID3D11Buffer;
class JSONData;

class Model
{
public:
	Model(void);
	~Model(void);

	void Deserialize(const JSONData& source);

public:
	std::string m_vertexLayout;
	CullMode m_cullMode;
	std::string m_mesh;
	ID3D11Buffer* m_vertexBuffer;
};

#endif
