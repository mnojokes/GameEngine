Texture2D <float4> Texture : register(t0);
SamplerState Sampler : register(s0);

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

float4 PS(OUT input) : SV_Target0
{
	float4 sampledColor = saturate(Texture.Sample(Sampler, input.tex_coordinates));
	float4 gammaCorrectedColor = float4(pow(sampledColor.xyz, 1.0f / 2.2f), sampledColor.w);
	return input.color * gammaCorrectedColor;
}
