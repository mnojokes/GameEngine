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

#include "RenderTarget.h"
#include "DirectXUtil.h"

ClearColor::ClearColor(void)
	: m_array{ 0.0f, 0.0f, 0.0f, 1.0f }
{}

ClearColor::ClearColor(float R, float G, float B)
	: m_array{ R, G, B, 1.0f }
{}

ClearColor::ClearColor(float R, float G, float B, float A)
	: m_array{ R, G, B, A }
{}

RenderTarget::RenderTarget(void)
	: m_RTView(nullptr)
{}

RenderTarget::~RenderTarget(void)
{
	Destroy();
}

bool RenderTarget::Initialize(ID3D11Device* device, UINT width, UINT height, const ClearColor& clearColor)
{
	HRESULT result;

	m_clearColor = clearColor;

	// DXGI_FORMAT value used to initialize RenderTarget resources
	const DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Create Texture2D
	D3D11_TEXTURE2D_DESC desc;
	{
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1U;
		desc.ArraySize = 1U;
		desc.Format = format;
		desc.SampleDesc.Count = 1U;
		desc.SampleDesc.Quality = 0U;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0U;
		desc.MiscFlags = 0U;
	}

	result = device->CreateTexture2D(&desc, nullptr, &m_texture);
	if (result != S_OK)
	{
		fprintf(stderr, "RenderTarget::%s: Failed to create texture\n", __func__);
		return false;
	}
	
	return CreateViews_Internal(device, m_texture);
}

bool RenderTarget::Initialize(ID3D11Device* device, ID3D11Texture2D* texture, const ClearColor& clearColor)
{
	m_clearColor = clearColor;
	m_texture = texture;
	return CreateViews_Internal(device, texture);
}

void RenderTarget::Destroy(void)
{
	SAFE_RELEASE(m_RTView);
	Texture2D::Destroy();
}

void RenderTarget::Clear(ID3D11DeviceContext* deviceContext)
{
	deviceContext->ClearRenderTargetView(m_RTView, m_clearColor.m_array);
}

bool RenderTarget::CreateViews_Internal(ID3D11Device* device, ID3D11Texture2D* texture)
{
	if (m_texture != nullptr)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		texture->GetDesc(&textureDesc);

		HRESULT result;
		{	// Create Shader Resource View
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			{
				desc.Format = textureDesc.Format;
				D3D11_TEX2D_SRV tex2D;
				{
					tex2D.MostDetailedMip = 0U;
					tex2D.MipLevels = 1U;
				}
				desc.Texture2D = tex2D;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			}

			result = device->CreateShaderResourceView(m_texture, &desc, &m_SRView);
			if (result != S_OK)
			{
				fprintf(stderr, "RenderTarget::%s: Failed to create Shader Resource View\n", __func__);
				return false;
			}
		}
		{	// Create Render Target View
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			{
				desc.Format = textureDesc.Format;
				D3D11_TEX2D_RTV tex2D;
				{
					tex2D.MipSlice = 0U;
				}
				desc.Texture2D = tex2D;
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			}

			result = device->CreateRenderTargetView(m_texture, &desc, &m_RTView);
			if (result != S_OK)
			{
				fprintf(stderr, "RenderTarget::%s: Failed to create Render Target View\n", __func__);
				return false;
			}
		}

		return true;
	}
	return false;
}
