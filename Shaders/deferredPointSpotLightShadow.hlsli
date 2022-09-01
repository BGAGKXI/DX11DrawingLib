//--------------------------------------------
//	テクスチャ/サンプラー
//--------------------------------------------

Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D PositionTexture : register(t2);
Texture2D SpecularTexture : register(t3);
Texture2D ShadowTexture : register(t4);
SamplerState DecalSampler : register(s0);
SamplerState ShadowSampler : register(s1);


//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major float4x4 World;
	row_major float4x4 matWVP;
};

cbuffer CBPerFrame : register(b2)
{
	float4	LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4	AmbientColor;	//環境光
	float4  EyePos;			//カメラ座標
};

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};

//バーテックスシェーダー出力構造体
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//ワールド法線
	float3 wPos : TEXCOORD2;	//ワールド座標
	float2 Tex : TEXCOORD3;
};

struct PSOutput
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
};

