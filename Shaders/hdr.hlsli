//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
static float3 screen_color = { 0.2,1.0,0.8 };

cbuffer CBPerFrame2 : register(b2)
{
	float4		LightColor;	//ライトの色
	float4		LightDir;		//ライトの方向
	float4		AmbientColor;	//環境光
	float4		EyePos;		//カメラ
};

cbuffer CBPerFrame4 : register(b4)
{
	float		Threshold;		//ブルーム用閾値
	float		dummy0;
	float		dummy1;
	float		dummy2;
}

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

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};
//--------------------------------------------
//	関数
//--------------------------------------------
//YIQ変換
float getBrightness(float3 color) {
	float3 y = { 0.299,0.587,0.114 };
	float bright = dot(y, color);
	return bright;
}

