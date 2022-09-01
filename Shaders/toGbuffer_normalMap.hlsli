//--------------------------------------------
//	テクスチャ・サンプラ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
Texture2D NormalTexture : register(t1);
SamplerState DecalSampler : register(s0);
//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major matrix  World;
	row_major matrix  matWVP;
};
cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4	AmbientColor;	//環境光
	float4  EyePos;		//カメラ座標
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
	float4 Position  	:SV_POSITION;
	float4 Color		:COLOR0;
	float3 wNormal		:TEXCOORD1;
	float3 wPos			:TEXCOORD2;
	float2 Tex			:TEXCOORD3;
	float3 vTan			:TEXCOORD4;
	float3 vBin			:TEXCOORD5;
	float3 vNor			:TEXCOORD6;
};

struct PSOutput
{
	float4 Color : SV_TARGET0;
	float4 wNormal : SV_TARGET1;
	float4 wPosition : SV_TARGET2;
};
