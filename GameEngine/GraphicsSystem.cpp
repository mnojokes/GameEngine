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

#include "GameObjectFactory.h"
#include "WorldManager.h"
#include "GraphicsSystem.h"
#include "Camera.h"
#include "Macros.h"

#include "GraphicsComponent.h"
#include "SceneComponent.h"
#include "ConstantBuffers.h"
#include "App.h"
#include "MessageFileRequest.h"
#include "MessagesGraphicsSystem.h"
#include "Texture2D.h"
#include "Material.h"
#include "Model.h"
#include "RenderTarget.h"
#include "VertexDeclarations.h"
#include "JSONUtility.h"
#include "JSONData.h"
#include <imgui_impl_dx11.h>
#include <vector>

GraphicsSystem::GraphicsSystem(App* app, GameObjectFactory* GOF)
	: ISystem(app, GOF)
	, m_DXGIFactory(nullptr)
	, m_device(nullptr)
	, m_deviceContext(nullptr)
	, m_swapChain(nullptr)
	, m_backBuffer(new RenderTarget)
	, m_worldManager(nullptr)
{}

GraphicsSystem::~GraphicsSystem(void)
{
	delete m_backBuffer;
}

bool GraphicsSystem::Initialize(INIReader* ini)
{
	FileRequestMessage frm;
	frm.m_FileLoader = std::bind(&GraphicsSystem::ReadTextureData, this, std::placeholders::_1);
	frm.m_extension = ".bmp";
	m_messenger.Send(frm);
	frm.m_extension = ".jpg";
	m_messenger.Send(frm);
	frm.m_extension = ".png";
	m_messenger.Send(frm);

	frm.m_FileLoader = std::bind(&GraphicsSystem::ReadMaterialData, this, std::placeholders::_1);
	frm.m_extension = ".mtrl";
	m_messenger.Send(frm);

	frm.m_FileLoader = std::bind(&GraphicsSystem::ReadModelData, this, std::placeholders::_1);
	frm.m_extension = ".mdl";
	m_messenger.Send(frm);

	m_worldManager = WorldManager::Get();

	HRESULT result;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_DXGIFactory));
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to initialize Graphics System\n", __func__);
		return false;
	}

	D3D_FEATURE_LEVEL D3DFeatureLevel[]{ D3D_FEATURE_LEVEL_11_0 };

	IDXGIAdapter* adapter = nullptr;
	{	// find and select suitable adapter
		IDXGIAdapter* tempAdapter = nullptr;
		DXGI_ADAPTER_DESC desc;
		UINT i = 0U;
		while ((result = m_DXGIFactory->EnumAdapters(i, &tempAdapter)) != DXGI_ERROR_NOT_FOUND)
		{
			tempAdapter->GetDesc(&desc);
			if (adapter != nullptr)
			{
				SAFE_RELEASE(adapter); // release previously selected adapter
			}
			adapter = tempAdapter;
			if (desc.VendorId == 0x10de || // nVidia
				desc.VendorId == 0x1002 || // AMD
				desc.VendorId == 0x1022 || // AMD
				desc.VendorId == 0x1414) // MS capture adapter
			{
				break; // best adapter found, stop search
			}

			++i;
		}
		if (adapter == nullptr)
		{
			fprintf(stderr, "GraphicsSystem::%s: No suitable graphics adapter found\n", __func__);
			return false;
		}
	}

	uint32_t deviceFlag = 0;

#if GRAPHICS_DEBUG
	deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL initializedD3DFeatureLevel;

	result = D3D11CreateDevice(
		adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		deviceFlag,
		D3DFeatureLevel,
		ARRAY_SIZE(D3DFeatureLevel),
		D3D11_SDK_VERSION,
		&m_device,
		&initializedD3DFeatureLevel,
		&m_deviceContext);	// initialize graphics device
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to initialize graphics device\n", __func__);
		return false;
	}

	SAFE_RELEASE(adapter);	// release graphics adapter

#if GRAPHICS_DEBUG
	ID3D11InfoQueue* infoQueue = nullptr;
	result = m_device->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&infoQueue));
	if (result == S_OK)
	{
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
		SAFE_RELEASE(infoQueue);
	}
	else
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to initialize device debug interface\n", __func__);
	}
#endif

	DXGI_SWAP_CHAIN_DESC SCDesc;
	{
		DXGI_MODE_DESC desc;
		desc.Width = 0U;
		desc.Height = 0U;
		desc.RefreshRate = { 0U, 0U };
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		
		SCDesc.BufferDesc = desc;
	}
	{
		DXGI_SAMPLE_DESC desc;
		desc.Count = 1U;
		desc.Quality = 0U;

		SCDesc.SampleDesc = desc;
	}
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT; // can render to buffer, be used in shaders
	SCDesc.BufferCount = 3U;
	SCDesc.OutputWindow = m_appPtr->GetWindow().GetHandle();
	SCDesc.Windowed = true;
	SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// allow changing resolution based on fullscreen/windowed dimensions

	result = m_DXGIFactory->CreateSwapChain(m_device, &SCDesc, &m_swapChain);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to create swap chain\n", __func__);
		return false;
	}

	//Initialize ImGui
	ImGui_ImplDX11_Init(m_device, m_deviceContext);

	// Initialize COM base - required to call DirectXTex routines
	result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to initialize COM library\n", __func__);
		return false;
	}

	// Creating rasterizer states
	ID3D11RasterizerState* state = CreateRasterizerState<FillMode::Wireframe, CullMode::Back, true>();
	if (state != nullptr)
	{
		m_rasterizerStates.insert({ "Wireframe", state });
	}
	state = CreateRasterizerState<FillMode::Solid, CullMode::Back, true>();
	if (state != nullptr)
	{
		m_rasterizerStates.insert({ "Solid", state });
	}

	// Creating blend states
	ID3D11BlendState* bState = CreateBlendState<
		false,
		BlendType::DestAlpha,
		BlendType::DestAlpha, 
		BlendType::DestAlpha, 
		BlendType::DestAlpha,
		ChannelMask::All>();
	if (bState != nullptr)
	{
		m_blendStates.insert({ "NoBlend", bState });
	}
	
	bState = CreateBlendState<
		true,
		BlendType::One,
		BlendType::One,
		BlendType::SrcAlpha,
		BlendType::DestAlpha,
		ChannelMask::All>();
	if (bState != nullptr)
	{
		m_blendStates.insert({ "AdditiveBlend", bState });
	}

	bState = CreateBlendState<
		true,
		BlendType::SrcAlpha,
		BlendType::InvSrcAlpha,
		BlendType::SrcAlpha,
		BlendType::InvSrcAlpha,
		ChannelMask::All>();
	if (bState != nullptr)
	{
		m_blendStates.insert({ "AlphaBlend", bState });
	}

	ID3D11SamplerState* sState = CreateSamplerState<SamplerType::Bilinear, TextureAddressMode::Wrap>();
	if (sState != nullptr)
	{
		m_samplerStates.insert({ "Simple2DSState", sState });
	}

	std::string shadersPath = m_appPtr->GetAssetPath() + "Shaders/";
	std::string vertexShaderPath = shadersPath + "VertexTexCoordColorVS.hlsl";
	std::string pixelShaderPath = shadersPath + "VertexTexCoordColorPS.hlsl";

	if (!CreateShader<ShaderType::Vertex>(vertexShaderPath))
	{
		return false;
	}

	if (!CreateShader<ShaderType::Pixel>(pixelShaderPath))
	{
		return false;
	}

	// Create input element description for VertexTexCoordColor layout
	D3D11_INPUT_ELEMENT_DESC inputElementPCTDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector4) + sizeof(uint32_t), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	auto iter = m_vertexShaderBlobs.find("VertexTexCoordColorVS");
	if (iter == m_vertexShaderBlobs.end())
	{
		fprintf(stderr, "GraphicsSystem::%s: can't find vertex shader blob\n", __func__);
		return false;
	}

	ID3D11InputLayout* inputLayout = nullptr;
	result = m_device->CreateInputLayout(
		inputElementPCTDesc,
		ARRAY_SIZE(inputElementPCTDesc),
		iter->second->GetBufferPointer(),
		iter->second->GetBufferSize(),
		&inputLayout);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to create input layout\n", __func__);
		return false;
	}

	m_inputLayouts.insert({ "VertexTexCoordColor", inputLayout });

	// TODO: Need to add 2 more vertex shader to represent vertex types
	//// Create input element description for VertexColor layout
	//D3D11_INPUT_ELEMENT_DESC inputElementPCDesc[] =
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 1, sizeof(Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0},
	//};

	//result = m_device->CreateInputLayout(
	//	inputElementPCDesc,
	//	ARRAY_SIZE(inputElementPCDesc),
	//	shaderBlob->GetBufferPointer(),
	//	shaderBlob->GetBufferSize(),
	//	&inputLayout);
	//if (result != S_OK)
	//{
	//	fprintf(stderr, "GraphicsSystem::%s: failed to create input layout\n", __func__);
	//	return false;
	//}

	//m_inputLayouts.insert({ "VertexColor", inputLayout });

	//// Create input element description for VertexTexCoord layout
	//D3D11_INPUT_ELEMENT_DESC inputElementPTDesc[] =
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, sizeof(Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0}
	//};

	//result = m_device->CreateInputLayout(
	//	inputElementPTDesc,
	//	ARRAY_SIZE(inputElementPTDesc),
	//	shaderBlob->GetBufferPointer(),
	//	shaderBlob->GetBufferSize(),
	//	&inputLayout);
	//if (result != S_OK)
	//{
	//	fprintf(stderr, "GraphicsSystem::%s: failed to create input layout\n", __func__);
	//	return false;
	//}

	//m_inputLayouts.insert({ "VertexTexCoord", inputLayout });

	ID3D11Buffer* buffer = CreateConstantBuffer(sizeof(CameraBuffer));
	if (buffer == nullptr)
	{
		return false;
	}

	m_constantBuffers.insert({ "CameraBuffer", buffer });

	buffer = CreateConstantBuffer(sizeof(MeshBuffer));
	if (buffer == nullptr)
	{
		return false;
	}

	m_constantBuffers.insert({ "MeshBuffer", buffer });

	return true;
}

void GraphicsSystem::Update(float deltaTime)
{
	m_graphicsComponents.clear();
	m_renderPackages.clear();

	Camera* camera = m_worldManager->GetActiveWorld()->GetCamera();
	if (camera != nullptr)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubr;
		std::map<std::string, ID3D11Buffer*>::iterator bufferIter = m_constantBuffers.find("CameraBuffer");

		HRESULT result = m_deviceContext->Map(bufferIter->second, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mappedSubr);
		if (result == S_OK)
		{
			CameraBuffer* buffer = static_cast<CameraBuffer*>(mappedSubr.pData);
			buffer->m_view = camera->GetViewMatrix();
			buffer->m_proj = m_projectionMatrix;
			buffer->m_viewProj = buffer->m_proj * buffer->m_view;
			buffer->m_invView = buffer->m_view.Inverse();
			buffer->m_invProj = buffer->m_proj.Inverse();
			buffer->m_invViewProj = buffer->m_invView * buffer->m_invProj;

			m_deviceContext->Unmap(bufferIter->second, 0U);
		}
		else
		{
			fprintf(stderr, "GraphicsSystem::%s: failed to map CameraBuffer\n", __func__);
		}

		const std::vector<GameObject*>& objects = m_GOF->GetObjectList();
		std::vector<GraphicsComponent*> graphics;
		std::vector<SceneComponent*> scene;

		for (GameObject* obj : objects)
		{
			obj->QueryComponents(graphics);
			m_graphicsComponents.insert(m_graphicsComponents.end(), graphics.begin(), graphics.end());
			obj->QueryComponents(scene);

			if (!scene.empty())
			{
				for (GraphicsComponent* graphicsComp : graphics)
				{
					if (graphicsComp->IsInitialized())
					{
						RenderPackage renderPackage = { graphicsComp, scene[0] };
						m_renderPackages.push_back(renderPackage);
					}
				}
			}
			else
			{
				fprintf(stderr, "GraphicsSystem::%s: %s does not contain SceneComponent\n",
					__func__, obj->GetObjectTypeName().c_str());
			}

			graphics.clear();
			scene.clear();
		}
	}

	Render(deltaTime);
}

void GraphicsSystem::Exit(void)
{
	for (std::pair<std::string, ID3D11SamplerState*> it : m_samplerStates)
	{
		SAFE_RELEASE(it.second);
	}
	m_samplerStates.clear();
	for (std::pair<std::string, ID3D11Buffer*> it : m_constantBuffers)
	{
		SAFE_RELEASE(it.second);
	}
	m_constantBuffers.clear();

	for (std::pair<std::string, ID3D11PixelShader*> it : m_pixelShaders)
	{
		SAFE_RELEASE(it.second);
	}
	m_pixelShaders.clear();

	for (std::pair<std::string, ID3D11VertexShader*> it : m_vertexShaders)
	{
		SAFE_RELEASE(it.second);
	}
	m_vertexShaders.clear();

	for (std::pair<std::string, ID3DBlob*> it : m_vertexShaderBlobs)
	{
		SAFE_RELEASE(it.second);
	}
	m_vertexShaderBlobs.clear();

	for (std::pair<std::string, ID3D11InputLayout*> it : m_inputLayouts)
	{
		SAFE_RELEASE(it.second);
	}
	m_inputLayouts.clear();

	for (std::pair<std::string, ID3D11BlendState*> it : m_blendStates)
	{
		SAFE_RELEASE(it.second);
	}
	m_blendStates.clear();

	for (std::pair<std::string, ID3D11RasterizerState*> it : m_rasterizerStates)
	{
		SAFE_RELEASE(it.second);
	}
	m_rasterizerStates.clear();

	for (std::pair<std::string, Material*> it : m_materials)
	{
		delete it.second;
	}
	m_materials.clear();

	for (std::pair<std::string, Texture2D*> it : m_textures)
	{
		delete it.second;
	}
	m_textures.clear();

	for (std::pair<std::string, Model*> it : m_models)
	{
		delete it.second;
	}
	m_models.clear();
	
	ImGui_ImplDX11_Shutdown();

	CoUninitialize();	// uninitialize COM base
	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_deviceContext);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_DXGIFactory);

	m_worldManager = nullptr;
}

void GraphicsSystem::RegisterMessages(void)
{
	m_messenger.RegisterMessage<WindowResizeMessage>(this);
	m_messenger.RegisterMessage<MaterialRequestMessage>(this);
	m_messenger.RegisterMessage<ModelRequestMessage>(this);
	m_messenger.RegisterMessage<AssignTextureToMaterialMessage>(this);
	m_messenger.RegisterMessage<AssignMeshToModelMessage>(this);
}

void GraphicsSystem::ProcessMessage(const Message* message)
{
	if (message->IsType<WindowResizeMessage>())
	{
		const WindowResizeMessage* msg = static_cast<const WindowResizeMessage*>(message);
		ResizeSwapChain(msg->m_width, msg->m_height);
		m_projectionMatrix = Matrix::PerspectiveLH(msg->m_width, msg->m_height, 0.1f, 100.0f);
	}
	else if (message->IsType<MaterialRequestMessage>())
	{
		const MaterialRequestMessage* msg = static_cast<const MaterialRequestMessage*>(message);
		std::map<std::string, Material*>::const_iterator iter = m_materials.find(msg->m_materialName);
		if (iter != m_materials.end())
		{
			msg->m_componentRef.m_material = new Material(*iter->second);
			if (msg->m_componentRef.m_model != nullptr)
			{
				msg->m_componentRef.SetInitialized(true);
			}
		}
	}
	else if (message->IsType<AssignTextureToMaterialMessage>())
	{
		const AssignTextureToMaterialMessage* msg = static_cast<const AssignTextureToMaterialMessage*>(message);
		std::map<std::string, Material*>::const_iterator materialIter = m_materials.find(msg->m_materialName);
		if (materialIter != m_materials.end())
		{
			std::map<std::string, Texture2D*>::const_iterator textureIter = m_textures.find(msg->m_textureName);
			if (textureIter != m_textures.end())
			{
				materialIter->second->m_texture = textureIter->second;
			}
			else
			{
				fprintf(stderr, "GraphicsSystem::%s: texture \"%s\" not found\n", __func__, msg->m_textureName.c_str());
			}
		}
		else
		{
			fprintf(stderr, "GraphicsSystem::%s: material \"%s\" not found\n", __func__, msg->m_materialName.c_str());
		}
	}
	else if (message->IsType<AssignMeshToModelMessage>())
	{
		const AssignMeshToModelMessage* msg = static_cast<const AssignMeshToModelMessage*>(message);
		std::map<std::string, Model*>::const_iterator modelIter = m_models.find(msg->m_modelName);
		if (modelIter != m_models.end())
		{
			// Hack - not loading from file
			// Create Vertex buffers
			ID3D11Buffer* buffer = nullptr;
			if (msg->m_modelName == "VertexColorQuad")
			{
				VertexColor vertexColor[6];
				vertexColor[0].m_position = { -0.5f, 0.5f, 0.0f };
				vertexColor[1].m_position = { -0.5f, -0.5f, 0.0f };
				vertexColor[2].m_position = { 0.5f, 0.5f, 0.0f };
				vertexColor[3].m_position = { -0.5f, -0.5f, 0.0f };
				vertexColor[4].m_position = { 0.5f, -0.5f, 0.0f };
				vertexColor[5].m_position = { 0.5f, 0.5f, 0.0f };

				vertexColor[0].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexColor[1].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexColor[2].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexColor[3].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexColor[4].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexColor[5].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

				buffer = CreateVertexBuffer(6U, vertexColor);
				if (buffer == nullptr)
				{
					fprintf(stderr, "GraphicsSystem::%s: Failed to create VertexColor buffer\n", __func__);
					return;
				}

				modelIter->second->m_vertexBuffer = buffer;
			}
			if (msg->m_modelName == "VertexTexCoordQuad")
			{
				VertexTexCoord vertexTexCoord[6];
				vertexTexCoord[0].m_position = { -0.5f, 0.5f, 0.0f };
				vertexTexCoord[1].m_position = { -0.5f, -0.5f, 0.0f };
				vertexTexCoord[2].m_position = { 0.5f, 0.5f, 0.0f };
				vertexTexCoord[3].m_position = { -0.5f, -0.5f, 0.0f };
				vertexTexCoord[4].m_position = { 0.5f, -0.5f, 0.0f };
				vertexTexCoord[5].m_position = { 0.5f, 0.5f, 0.0f };

				vertexTexCoord[0].m_texCoord = { 0.0f, 0.0f };
				vertexTexCoord[1].m_texCoord = { 0.0f, 1.0f };
				vertexTexCoord[2].m_texCoord = { 1.0f, 0.0f };
				vertexTexCoord[3].m_texCoord = { 0.0f, 1.0f };
				vertexTexCoord[4].m_texCoord = { 1.0f, 1.0f };
				vertexTexCoord[5].m_texCoord = { 1.0f, 0.0f };

				buffer = CreateVertexBuffer(6U, vertexTexCoord);
				if (buffer == nullptr)
				{
					fprintf(stderr, "GraphicsSystem::%s: Failed to create VertexTexCoord buffer\n", __func__);
					return;
				}

				modelIter->second->m_vertexBuffer = buffer;
			}
			if (msg->m_modelName == "VertexTexCoordColorQuad")
			{
				VertexTexCoordColor vertexTexCoordColor[6];
				vertexTexCoordColor[0].m_position = { -0.5f, 0.5f, 0.0f };
				vertexTexCoordColor[1].m_position = { -0.5f, -0.5f, 0.0f };
				vertexTexCoordColor[2].m_position = { 0.5f, 0.5f, 0.0f };
				vertexTexCoordColor[3].m_position = { -0.5f, -0.5f, 0.0f };
				vertexTexCoordColor[4].m_position = { 0.5f, -0.5f, 0.0f };
				vertexTexCoordColor[5].m_position = { 0.5f, 0.5f, 0.0f };

				vertexTexCoordColor[0].m_texCoord = { 0.0f, 0.0f };
				vertexTexCoordColor[1].m_texCoord = { 0.0f, 1.0f };
				vertexTexCoordColor[2].m_texCoord = { 1.0f, 0.0f };
				vertexTexCoordColor[3].m_texCoord = { 0.0f, 1.0f };
				vertexTexCoordColor[4].m_texCoord = { 1.0f, 1.0f };
				vertexTexCoordColor[5].m_texCoord = { 1.0f, 0.0f };

				vertexTexCoordColor[0].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexTexCoordColor[1].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexTexCoordColor[2].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexTexCoordColor[3].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexTexCoordColor[4].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertexTexCoordColor[5].m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

				buffer = CreateVertexBuffer(6U, vertexTexCoordColor);
				if (buffer == nullptr)
				{
					fprintf(stderr, "GraphicsSystem::%s: Failed to create VertexTexCoordColor buffer\n", __func__);
					return;
				}

				modelIter->second->m_vertexBuffer = buffer;
			}
		}
	}
	else if (message->IsType<ModelRequestMessage>())
	{
		const ModelRequestMessage* msg = static_cast<const ModelRequestMessage*>(message);
		std::map<std::string, Model*>::const_iterator iter = m_models.find(msg->m_modelName);
		if (iter != m_models.end())
		{
			msg->m_componentRef.m_model = iter->second;
			if (msg->m_componentRef.m_material != nullptr)
			{
				msg->m_componentRef.SetInitialized(true);
			}
		}
	}
}

void GraphicsSystem::RegisterComponents(void) const
{
	GraphicsComponent::Register();
}

ID3D11Buffer* GraphicsSystem::CreateConstantBuffer(UINT size, const void* data) const
{
	HRESULT result;
	ID3D11Buffer* buffer = nullptr;

	if (size == 0U)
	{
		size = alignment;
	}
	else
	{
		int padding = alignment - (size % alignment);
		if (padding != alignment)
		{
			size += padding;
		}
	}

	D3D11_BUFFER_DESC desc;
	{
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0U;
		desc.StructureByteStride = 0U;
	}

	D3D11_SUBRESOURCE_DATA* subrPtr = nullptr;

	if (data != nullptr)
	{
		D3D11_SUBRESOURCE_DATA subr;
		{
			subr.pSysMem = data;
			subr.SysMemPitch = 0U;
			subr.SysMemSlicePitch = 0U;
		}
		subrPtr = &subr;
	}
	result = m_device->CreateBuffer(&desc, subrPtr, &buffer);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to create buffer\n", __func__);
		return nullptr;
	}

	return buffer;
}

D3D11_BLEND GraphicsSystem::ConvertToD3D11BlendType(BlendType blend)
{
	switch (blend)
	{
	case BlendType::Zero:
		return D3D11_BLEND_ZERO;
	case BlendType::One:
		return D3D11_BLEND_ONE;
	case BlendType::SrcAlpha:
		return D3D11_BLEND_SRC_ALPHA;
	case BlendType::DestAlpha:
		return D3D11_BLEND_DEST_ALPHA;
	case BlendType::InvSrcAlpha:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case BlendType::InvDestAlpha:
		return D3D11_BLEND_INV_DEST_ALPHA;
	default:
		fprintf(stderr, "GraphicsSystem::%s: unsupported blend type\n", __func__);
		break;
	}

	return D3D11_BLEND_ZERO;
}

bool GraphicsSystem::LoadTexture(const std::string& file, DirectX::ScratchImage& image) const
{
	std::wstring path(file.begin(), file.end());	// convert file path to wstring

	HRESULT result = DirectX::LoadFromWICFile(
		path.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image);

	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to load texture from file %s\n", __func__, file.c_str());
		return false;
	}
	return true;
}

void GraphicsSystem::ReadTextureData(const std::string& filePath)
{
	std::string texName = StringUtility::ExtractFilenameFromPath(filePath);
	texName = StringUtility::RemoveFileExtension(texName);

	if (m_textures.find(texName) == m_textures.end())
	{
		DirectX::ScratchImage scrImg;
		LoadTexture(filePath, scrImg);
		Texture2D* tex = new Texture2D;
		tex->Initialize(m_device, scrImg);
		m_textures.insert(std::make_pair(texName, tex));
	}
	else
	{
		fprintf(stderr, "GraphicsSystem::%s: \"%s\" already exists\n", __func__, texName.c_str());
	}
}

void GraphicsSystem::ReadMaterialData(const std::string& filePath)
{
	std::string matName = StringUtility::ExtractFilenameFromPath(filePath);
	matName = StringUtility::RemoveFileExtension(matName);

	if (m_materials.find(matName) == m_materials.end())
	{
		JSONDoc* materialDoc = JSONUtility::ParseFile(filePath);
		if (materialDoc != nullptr)
		{
			JSONData materialData(materialDoc->MemberBegin());
			if (materialData.IsValid())
			{
				Material* material = new Material;
				material->Deserialize(materialData);
				m_materials.insert({ matName, material });
				AssignTextureToMaterialMessage msg(matName, material->m_textureName);
				m_messenger.Send(msg);
			}
			else
			{
				fprintf(stderr, "GraphicsSystem::%s: file \"%s\" does not contain valid Material data\n",
					__func__, StringUtility::ExtractFilenameFromPath(filePath).c_str());
			}
			delete materialDoc;
		}
	}
	else
	{
		fprintf(stderr, "GraphicsSystem::%s: \"%s\" already exists\n", __func__, matName.c_str());
	}
}

void GraphicsSystem::ReadModelData(const std::string& filePath)
{
	std::string modName = StringUtility::ExtractFilenameFromPath(filePath);
	modName = StringUtility::RemoveFileExtension(modName);

	if (m_models.find(modName) == m_models.end())
	{
		JSONDoc* modelDoc = JSONUtility::ParseFile(filePath);
		if (modelDoc != nullptr)
		{
			JSONData modelData(modelDoc->MemberBegin());
			if (modelData.IsValid())
			{
				Model* model = new Model;
				model->Deserialize(modelData);
				m_models.insert({ modName, model });
				AssignMeshToModelMessage msg(modName, model->m_mesh);
				m_messenger.Send(msg);
			}
			else
			{
				fprintf(stderr, "GraphicsSystem::%s: file \"%s\" does not contain valid Model data\n",
					__func__, StringUtility::ExtractFilenameFromPath(filePath).c_str());
			}
			delete modelDoc;
		}
	}
	else
	{
		fprintf(stderr, "GraphicsSystem::%s: \"%s\" already exists\n", __func__, modName.c_str());
	}
}

void GraphicsSystem::Render(float deltaTime) const
{
	m_backBuffer->Clear(m_deviceContext);
	RenderPass(deltaTime);
	RenderPassUI();

	m_swapChain->Present(0, 0);
}

void GraphicsSystem::RenderPass(float deltaTime) const
{
	if (m_renderPackages.empty())
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubr;
	auto meshBuffer = m_constantBuffers.find("MeshBuffer");
	auto cameraBuffer = m_constantBuffers.find("CameraBuffer");
	// TODO safety checks

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = m_windowSize.x;
	viewport.Height = m_windowSize.y;
	viewport.MinDepth = viewport.MaxDepth = 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);



	auto samplerState = m_samplerStates.find("Simple2DSState");
	if (samplerState == m_samplerStates.end())
	{
		fprintf(stderr, "GraphicsSystem::%s: no valid sampler state found\n", __func__);
		return;
	}

	m_deviceContext->PSSetSamplers(0, 1, &samplerState->second);

	auto rasterizerState = m_rasterizerStates.find("Solid");
	if (rasterizerState == m_rasterizerStates.end())
	{
		fprintf(stderr, "GraphicsSystem::%s: no valid rasterizer state found\n", __func__);
		return;
	}

	m_deviceContext->RSSetState(rasterizerState->second);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Hack - should come from graphics component model
	auto inputLayout = m_inputLayouts.find("VertexTexCoordColor");
	if (inputLayout == m_inputLayouts.end())
	{
		fprintf(stderr, "GraphicsSystem::%s: no valid input layout found\n", __func__);
		return;
	}

	m_deviceContext->IASetInputLayout(inputLayout->second);

	m_deviceContext->OMSetRenderTargets(1, &m_backBuffer->m_RTView, nullptr);

	auto pixelShader = m_pixelShaders.find("VertexTexCoordColorPS");
	if (pixelShader == m_pixelShaders.end())
	{
		fprintf(stderr, "GraphicsSystem::%s: no valid pixel shader found\n", __func__);
		return;
	}

	m_deviceContext->PSSetShader(pixelShader->second, nullptr, 0);

	auto vertexShader = m_vertexShaders.find("VertexTexCoordColorVS");
	if (vertexShader == m_vertexShaders.end())
	{
		fprintf(stderr, "GraphicsSystem::%s: no valid vertex shader found\n", __func__);
		return;
	}

	m_deviceContext->VSSetShader(vertexShader->second, nullptr, 0);

	for (const RenderPackage& rp : m_renderPackages)
	{
		const std::string& blendType = rp.m_graphicsComponent->m_material->m_blend;
		float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		auto blendState = m_blendStates.find(blendType);
		if (blendState == m_blendStates.end())
		{
			fprintf(stderr, "GraphicsSystem::%s: no valid blend state found\n", __func__);
			continue;
		}

		m_deviceContext->OMSetBlendState(blendState->second, blendFactor, 0xFFFFFFFF);

		HRESULT result = m_deviceContext->Map(meshBuffer->second, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mappedSubr);
		if (result == S_OK)
		{
			MeshBuffer* buffer = static_cast<MeshBuffer*>(mappedSubr.pData);
			buffer->m_world = rp.m_sceneComponent->GetTransform().GetTransformMatrix();
			buffer->m_color = rp.m_graphicsComponent->m_material->m_color;

			m_deviceContext->Unmap(meshBuffer->second, 0U);
		}
		else
		{
			fprintf(stderr, "GraphicsSystem::%s: failed to map MeshBuffer for %s\n",
				__func__, rp.m_graphicsComponent->GetOwner()->GetObjectTypeName().c_str());
		}

		// Hack - should be serialized on mesh
		UINT32 vbSize = sizeof(VertexTexCoordColor);
		UINT32 offset = 0;

		m_deviceContext->IASetVertexBuffers(0, 1, &rp.m_graphicsComponent->m_model->m_vertexBuffer, &vbSize, &offset);

		m_deviceContext->PSSetConstantBuffers(0, 1, &cameraBuffer->second);

		ID3D11Buffer* buffers[] = { cameraBuffer->second, meshBuffer->second };
		m_deviceContext->VSSetConstantBuffers(0, 2, buffers);

		m_deviceContext->PSSetShaderResources(0, 1, &rp.m_graphicsComponent->m_material->m_texture->m_SRView);

		m_deviceContext->Draw(6, 0);
	}
}

void GraphicsSystem::RenderPassUI(void) const
{
	m_deviceContext->OMSetRenderTargets(1, &m_backBuffer->m_RTView, nullptr);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool GraphicsSystem::ResizeSwapChain(uint32_t width, uint32_t height)
{
	if (width == 0U || height == 0U)
	{
		return true;
	}

	m_windowSize.x = static_cast<float>(width);
	m_windowSize.y = static_cast<float>(height);

	HRESULT result;

	m_backBuffer->Destroy();
	result = m_swapChain->ResizeBuffers(0U, width, height, DXGI_FORMAT_UNKNOWN, 0U);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: failed to resize buffers\n", __func__);
		return false;
	}

	ID3D11Texture2D* surface;
	result = m_swapChain->GetBuffer(0U, __uuidof(ID3D11Texture2D), (void**)&surface);
	if (result != S_OK)
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to get surface\n", __func__);
		return false;
	}

	bool initialized = m_backBuffer->Initialize(m_device, surface, ClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	if (!initialized)
	{
		fprintf(stderr, "GraphicsSystem::%s: Failed to initialize back buffer\n", __func__);
		return false;
	}

	return true;
}

ID3D11Buffer* GraphicsSystem::CreateVertexBuffer_Internal(UINT size, const void* data) const
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	{
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0U;
		desc.MiscFlags = 0U;
		desc.StructureByteStride = 0U;
	}

	D3D11_SUBRESOURCE_DATA subr;
	{
		subr.pSysMem = data;
		subr.SysMemPitch = 0U;
		subr.SysMemSlicePitch = 0U;
	}

	HRESULT result = m_device->CreateBuffer(&desc, &subr, &buffer);
	if (result != S_OK)
	{
		fprintf(stderr, "%s: failed to create buffer\n", __func__);
		return nullptr;
	}

	return buffer;
}

void GraphicsSystem::PrintShaderError(ID3DBlob* errorBlob)
{
	char* errMsg = static_cast<char*>(errorBlob->GetBufferPointer());
	size_t size = errorBlob->GetBufferSize();

	char* error = new char[size + 1U];
	memcpy(error, errMsg, size);
	error[size] = '\0';

	fprintf(stderr, "GraphicsSystem: %s\n", error);

	delete[] error;
}
