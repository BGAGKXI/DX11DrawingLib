//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

Texture2D RampTexture : register(t1);
SamplerState RampSampler : register(s1);


//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------
cbuffer	CBPerMesh	: register(b0)
{
	row_major matrix	World;
	row_major matrix matWVP;
};

cbuffer	CBPerFrame2	: register(b2)
{
	float4	LightColor;		//	ライトの色
	float4	LightDir;		//	ライトの方向
	float4	AmbientColor;	//	環境光
	float4  EyePos;		//	カメラ座標
};

cbuffer CBPerFrame3 : register(b3)
{
	float4 Paramter; //	x = 輪郭線の太さ, yzw = 空き
}

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct	VSInput
{
	float3	Position	: POSITION;
	float3	Normal	: NORMAL;
	float2	Tex		: TEXCOORD;
};

//	バーテックスシェーダー出力構造体
struct	GSInput
{
	float3	Position	: POSITION;
	float3	Normal	: TEXCOORD1;
	float2	Tex		: TEXCOORD2;
	float4	Color		: COLOR0;
};

//	バーテックスシェーダー出力構造体
struct	PSInput
{
	
	float4	Position	: SV_POSITION;
	float3	wNormal	: TEXCOORD1;	//ワールド法線
	float3	wPosition	: TEXCOORD2;	//ワールド座標
	float2	Tex		: TEXCOORD3;
	float4	Color		: COLOR0;
	
};

