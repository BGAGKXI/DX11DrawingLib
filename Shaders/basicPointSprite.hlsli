//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------

cbuffer CBPerFrame : register(b0)
{
	row_major float4x4 View; // ビュー変換行列
	row_major float4x4 Projection; // 透視変換行列
};
//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Size     : TEXCOORD;
	float4 Color    : COLOR;
};

struct GSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Size     : TEXCOORD;
	float4 Color    : COLOR;
};

struct PSInput
{
	float4 Position  : SV_POSITION;// 頂点座標(射影座標系)
	float2 Tex  : TEXCOORD;
	float4 Color : COLOR0;
};
