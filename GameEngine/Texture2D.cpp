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

#include "Texture2D.h"
#include "DirectXUtil.h"
#include "GraphicsSystem.h"

Texture2D::Texture2D(void)
	: m_texture(nullptr)
	, m_SRView(nullptr)
{}

Texture2D::~Texture2D(void)
{
	Destroy();
}

bool Texture2D::Initialize(ID3D11Device* device, const DirectX::ScratchImage& image)
{
	HRESULT result;

	const DirectX::TexMetadata& meta = image.GetMetadata();
	{	// Create Texture2D
		D3D11_TEXTURE2D_DESC desc;
		{
			desc.Width = static_cast<UINT>(meta.width);
			desc.Height = static_cast<UINT>(meta.height);
			desc.MipLevels = static_cast<UINT>(meta.mipLevels);	// sets max value by default to create full chain of mips
			desc.ArraySize = static_cast<UINT>(meta.arraySize);
			desc.Format = meta.format;
			desc.SampleDesc.Count = 1U;
			desc.SampleDesc.Quality = 0U;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0U;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		D3D11_SUBRESOURCE_DATA subr;
		{
			subr.pSysMem = image.GetPixels();
			subr.SysMemPitch = static_cast<UINT>(image.GetImage(0, 0, 0)->rowPitch);	// use row pitch from Image
			subr.SysMemSlicePitch = 0U;
		}

		result = device->CreateTexture2D(&desc, &subr, &m_texture);
		if (result != S_OK)
		{
			fprintf(stderr, "Texture2D::%s: Failed to create texture\n", __func__);
			return false;
		}
	}

	if (m_texture != nullptr)
	{	// Create Shader Resource View
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		{
			desc.Format = meta.format;
			D3D11_TEX2D_SRV tex2D;
			{
				tex2D.MostDetailedMip = 0U;
				tex2D.MipLevels = -1;
			}
			desc.Texture2D = tex2D;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}

		result = device->CreateShaderResourceView(m_texture, &desc, &m_SRView);
		if (result != S_OK)
		{
			fprintf(stderr, "Texture2D::%s: Failed to create Shader Resource View\n", __func__);
			return false;
		}
	}
	return true;
}

void Texture2D::Destroy(void)
{
	SAFE_RELEASE(m_SRView);
	SAFE_RELEASE(m_texture);
}
