#define rcp255f 1.0f / 255.0f
#define UNORMALIZE(x) (float)(x) * rcp255f

struct IN
{
	float3 position : POSITION0;
	float4 color : COLOR0;
	float2 tex_coordinates : TEXCOORD0;
};

struct OUT
{
	float4 position : SV_Position;
	float4 color : COLOR0;
	float2 tex_coordinates : TEXCOORD0;
};

cbuffer CameraBuffer : register(b0)
{
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	row_major float4x4 m_viewProj;
	row_major float4x4 m_invView;
	row_major float4x4 m_invProj;
	row_major float4x4 m_invViewProj;
};

cbuffer MeshBuffer : register(b1)
{
	row_major float4x4 m_world;
	float2 m_uvOffset;
	uint m_color;
};

OUT VS(IN input)
{
	float4 worldPosition = mul(m_world, float4(input.position, 1.0f));
	float4 finalPosition = mul(m_viewProj, worldPosition);

	float r = UNORMALIZE(m_color & 0xFF);
	float g = UNORMALIZE((m_color >> 8) & 0xFF);
	float b = UNORMALIZE((m_color >> 16) & 0xFF);
	float a = UNORMALIZE((m_color >> 24) & 0xFF);

	OUT output;
	output.position = finalPosition;
	output.color = input.color * float4(r, g, b, a);
	output.tex_coordinates = input.tex_coordinates;
	
	return output;
}
