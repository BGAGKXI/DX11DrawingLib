//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);
//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------
cbuffer	CBPerMesh	: register(b0)
{
	matrix	World;
	matrix	matWVP;
};

cbuffer	CBPerFrame2	: register(b2)
{
	float4	LightColor;			//ライトの色
	float4	LightDir;			//ライトの方向
	float4	AmbientColor;		//環境光
	float4  EyePos;			//カメラ座標
};
cbuffer CBPerFrame3 : register(b4)
{

	float4	Parameter;			//x = 破壊レート
						//y = 拡縮係数
						//z = 回転係数
						//w = 法線方向への移動係数

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
