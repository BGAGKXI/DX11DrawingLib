//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	row_major float4x4  World;
	row_major float4x4	matWVP;
};
cbuffer CBPerFrame : register(b2)
{
	float4  	LightColor;		//ライトの色
	float4		LightDir;		//ライトの方向
	float4  	AmbientColor;	//環境光
	float4		EyePos;		//カメラ位置
};

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
};

struct GSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
	float3 wPos: TEXCOORD2;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
	float3 wNormal: TEXCOORD1;
	float3 wPos : TEXCOORD2;
};

