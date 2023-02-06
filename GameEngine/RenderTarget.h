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

#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "Texture2D.h"

// ClearColor: stores RGBA color data. Can be accessed as a float array or struct
struct ClearColor
{
	ClearColor(void);
	ClearColor(float R, float G, float B);
	ClearColor(float R, float G, float B, float A);

	union
	{
		float m_array[4];
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
	};
};

class RenderTarget : public Texture2D
{
public:
	RenderTarget(void);
	~RenderTarget(void);
	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator=(const RenderTarget&) = delete;

	bool Initialize(ID3D11Device* device, UINT width, UINT height, const ClearColor& clearColor);
	bool Initialize(ID3D11Device* device, ID3D11Texture2D* texture, const ClearColor& clearColor);
	void Destroy(void);

	void Clear(ID3D11DeviceContext* deviceContext);

private:
	bool CreateViews_Internal(ID3D11Device* device, ID3D11Texture2D* texture);

public:
	ID3D11RenderTargetView* m_RTView;
	ClearColor m_clearColor;
};

#endif
