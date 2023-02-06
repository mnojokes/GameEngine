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

#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H

#include "ISystem.h"
#include "FileSystem.h"
#include "DirectXTex.h"
#include "GraphicsDefines.h"
#include "StringUtility.h"
#include "Matrix.h"
#include "Vector2.h"
#include "DirectXUtil.h"
#include <D3DCompiler.h>

#ifdef _DEBUG
#define GRAPHICS_DEBUG 1
#else
#define GRAPHICS_DEBUG 0
#endif

class GameObjectFactory;
class WorldManager;
class GraphicsComponent;
class SceneComponent;
class Window;
class Texture2D;
class RenderTarget;
class Material;
class Model;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

struct RenderPackage
{
	GraphicsComponent* m_graphicsComponent = nullptr;
	SceneComponent* m_sceneComponent = nullptr;
};

namespace DirectX
{
	class ScratchImage;
};

const UINT alignment = 16U;

class GraphicsSystem : public ISystem
{
public:
	GraphicsSystem(App* app, GameObjectFactory* GOF);
	~GraphicsSystem(void);

	bool Initialize(INIReader* ini) override final;
	void Update(float deltaTime) override final;
	void Exit(void) override final;

	void RegisterMessages(void) override final;
	void ProcessMessage(const Message* message) override final;

	void RegisterComponents(void) const override final;

	ID3D11Buffer* CreateConstantBuffer(UINT size, const void* data = nullptr) const;
	template <typename Type>
	ID3D11Buffer* CreateVertexBuffer(UINT numVertices, const Type* data) const;

	template <FillMode fillMode, CullMode cullMode, bool counterclockwise>
	ID3D11RasterizerState* CreateRasterizerState(void);
	template<bool blendEnable, BlendType srcBlend, BlendType destBlend, BlendType srcAlpha, BlendType destAlpha, ChannelMask chMask>
	ID3D11BlendState* CreateBlendState(void);
	template<SamplerType samplerType, TextureAddressMode textureAddressMode>
	ID3D11SamplerState* CreateSamplerState(void);

	static D3D11_BLEND ConvertToD3D11BlendType(BlendType blend);

	bool LoadTexture(const std::string& file, DirectX::ScratchImage& image) const;
	bool ResizeSwapChain(uint32_t width, uint32_t height);

private:
	void ReadTextureData(const std::string& filePath);
	void ReadMaterialData(const std::string& filePath);
	void ReadModelData(const std::string& filePath);

	void Render(float deltaTime) const;
	void RenderPass(float deltaTime) const;
	void RenderPassUI(void) const;
	ID3D11Buffer* CreateVertexBuffer_Internal(UINT size, const void* data) const;
	void PrintShaderError(ID3DBlob* errorBlob);

	template <ShaderType shaderType>
	bool CreateShader(const std::string& path);

private:
	IDXGIFactory* m_DXGIFactory;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	IDXGISwapChain* m_swapChain;
	RenderTarget* m_backBuffer;
	Matrix m_projectionMatrix; // calculated once per window size, recalculated upon resize

	Vector2 m_windowSize;

	std::map<std::string, Texture2D*> m_textures;
	std::map<std::string, ID3D11SamplerState*> m_samplerStates;
	std::map<std::string, Model*> m_models;
	std::map<std::string, Material*> m_materials;
	std::map<std::string, ID3D11RasterizerState*> m_rasterizerStates;
	std::map<std::string, ID3D11BlendState*> m_blendStates;
	std::map<std::string, ID3D11InputLayout*> m_inputLayouts;
	std::map<std::string, ID3D11VertexShader*> m_vertexShaders;
	std::map<std::string, ID3D11PixelShader*> m_pixelShaders;
	std::map<std::string, ID3DBlob*> m_vertexShaderBlobs;
	std::map<std::string, ID3D11Buffer*> m_constantBuffers;
	std::vector<RenderPackage> m_renderPackages;

	std::vector<GraphicsComponent*> m_graphicsComponents; // stores graphics components for Update. Repopulates every frame.

	WorldManager* m_worldManager; // used for getting the Camera pointer from the currently active world
};

template <typename Type>
ID3D11Buffer* GraphicsSystem::CreateVertexBuffer(UINT numVertices, const Type* data) const
{
	UINT size = static_cast<UINT>(sizeof(Type)) * numVertices;
	return CreateVertexBuffer_Internal(size, data);
}

template <FillMode fillMode, CullMode cullMode, bool counterclockwise>
ID3D11RasterizerState* GraphicsSystem::CreateRasterizerState(void)
{
	D3D11_RASTERIZER_DESC desc {};
	desc.DepthClipEnable = true; // default value by documentation
	desc.FrontCounterClockwise = counterclockwise;

	switch (fillMode)
	{
	case FillMode::Wireframe:
		desc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	case FillMode::Solid:
		desc.FillMode = D3D11_FILL_SOLID;
		break;
	default:
		break;
	}

	switch (cullMode)
	{
	case CullMode::Front:
		desc.CullMode = D3D11_CULL_FRONT;
		break;
	case CullMode::Back:
		desc.CullMode = D3D11_CULL_BACK;
		break;
	case CullMode::None:
		desc.CullMode = D3D11_CULL_NONE;
		break;
	default:
		break;
	}
	
	ID3D11RasterizerState* rState = nullptr;

	HRESULT result = m_device->CreateRasterizerState(&desc, &rState);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to create rasterizer state\n", __func__);
	}

	return rState;
}

template<
	bool blendEnable,
	BlendType srcBlend,
	BlendType destBlend,
	BlendType srcAlpha,
	BlendType destAlpha,
	ChannelMask chMask>
ID3D11BlendState* GraphicsSystem::CreateBlendState(void)
{
	D3D11_BLEND_DESC desc{};
	desc.RenderTarget[0].BlendEnable = blendEnable;
	desc.RenderTarget[0].SrcBlend = ConvertToD3D11BlendType(srcBlend);
	desc.RenderTarget[0].DestBlend = ConvertToD3D11BlendType(destBlend);
	desc.RenderTarget[0].SrcBlendAlpha = ConvertToD3D11BlendType(srcAlpha);
	desc.RenderTarget[0].DestBlendAlpha = ConvertToD3D11BlendType(destAlpha);

	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	UINT8 channelMask = 0U;
	switch (chMask)
	{
	case ChannelMask::Red:
		channelMask = D3D11_COLOR_WRITE_ENABLE_RED;
		break;
	case ChannelMask::Green:
		channelMask = D3D11_COLOR_WRITE_ENABLE_GREEN;
		break;
	case ChannelMask::Blue:
		channelMask = D3D11_COLOR_WRITE_ENABLE_BLUE;
		break;
	case ChannelMask::Alpha:
		channelMask = D3D11_COLOR_WRITE_ENABLE_ALPHA;
		break;
	case ChannelMask::All:
		channelMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	default:
		break;
	}

	desc.RenderTarget[0].RenderTargetWriteMask = channelMask;

	ID3D11BlendState* state = nullptr;
	HRESULT result = m_device->CreateBlendState(&desc, &state);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to create blend state\n", __func__);
	}

	return state;
}

template<SamplerType samplerType, TextureAddressMode textureAddressMode>
ID3D11SamplerState* GraphicsSystem::CreateSamplerState(void)
{
	D3D11_SAMPLER_DESC desc{};

	D3D11_TEXTURE_ADDRESS_MODE dx11TextureAddressMode{};
	switch (textureAddressMode)
	{
	case TextureAddressMode::Wrap:
		dx11TextureAddressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case TextureAddressMode::Mirror:
		dx11TextureAddressMode = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case TextureAddressMode::Clamp:
		dx11TextureAddressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	default:
		break;
	}

	desc.AddressU = dx11TextureAddressMode;
	desc.AddressV = dx11TextureAddressMode;
	desc.AddressW = dx11TextureAddressMode;

	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MaxAnisotropy = 0;
	
	D3D11_FILTER dx11SamplerType{};
	switch (samplerType)
	{
	case SamplerType::Point:
		dx11SamplerType = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case SamplerType::Bilinear:
		dx11SamplerType = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case SamplerType::Trilinear:
		dx11SamplerType = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case SamplerType::Anisotropic1:
		desc.MaxAnisotropy = 1;
		dx11SamplerType = D3D11_FILTER_ANISOTROPIC;
		break;
	case SamplerType::Anisotropic4:
		desc.MaxAnisotropy = 4;
		dx11SamplerType = D3D11_FILTER_ANISOTROPIC;
		break;
	case SamplerType::Anisotropic16:
		desc.MaxAnisotropy = 16;
		dx11SamplerType = D3D11_FILTER_ANISOTROPIC;
		break;
	default:
		break;
	}

	desc.Filter = dx11SamplerType;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* state = nullptr;
	HRESULT result = m_device->CreateSamplerState(&desc, &state);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to create sampler state\n", __func__);
	}

	return state;
}

template <ShaderType shaderType>
bool GraphicsSystem::CreateShader(const std::string& path)
{
	std::string name = StringUtility::ExtractFilenameFromPath(path);
	name = StringUtility::RemoveFileExtension(name);
	const std::wstring wPath(path.begin(), path.end());
	std::string shaderTypeName;
	std::string shaderVersion;
	
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	switch (shaderType)
	{
	case ShaderType::Vertex:
		shaderTypeName = "VS";
		shaderVersion = "vs_5_0";
		break;
	case ShaderType::Pixel:
		shaderTypeName = "PS";
		shaderVersion = "ps_5_0";
		break;
	default:
		break;
	}

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT result = D3DCompileFromFile(
		wPath.c_str(),
		nullptr,
		nullptr,
		shaderTypeName.c_str(),
		shaderVersion.c_str(),
		flags,
		0,
		&shaderBlob,
		&errorBlob);

	if (result != S_OK)
	{
		if (errorBlob != nullptr)
		{
			PrintShaderError(errorBlob);
		}
		else
		{
			fprintf(stderr, "GraphicsSystem::%s: D3DCompileFromFile could not open %s\n", __func__, name.c_str());
		}
		return false;
	}

	switch (shaderType)
	{
	case ShaderType::Vertex:
	{
		ID3D11VertexShader* vertexShader = nullptr;
		result = m_device->CreateVertexShader(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			nullptr,
			&vertexShader);

		if (result != S_OK)
		{
			fprintf(stderr, "GraphicsSystem::%s: failed to create Vertex Shader\n", __func__);
			return false;
		}

		m_vertexShaders.insert({ name, vertexShader });
		break;
	}
	case ShaderType::Pixel:
	{
		ID3D11PixelShader* pixelShader = nullptr;
		result = m_device->CreatePixelShader(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			nullptr,
			&pixelShader);

		if (result != S_OK)
		{
			fprintf(stderr, "GraphicsSystem::%s: failed to create Pixel Shader\n", __func__);
			return false;
		}

		m_pixelShaders.insert({ name, pixelShader });
		break;
	}
	default:
		break;
	}

	if (shaderType == ShaderType::Vertex)
	{
		m_vertexShaderBlobs.insert({ name, shaderBlob });
	}
	else
	{
		SAFE_RELEASE(shaderBlob);
	}

	return true;
}

#endif
