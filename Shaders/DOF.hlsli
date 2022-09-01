Texture2D DiffuseTexture : register(t0);
Texture2D BokehTexture : register(t1);
Texture2D DepthTexture : register(t2);

SamplerState DecalSampler : register(s0);
SamplerState DepthSampler : register(s1);

cbuffer CBPerFrame3 : register(b4)
{
	float		dist;
	float		focus;
	float		dummy0;
	float		dummy1;
};

struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};
