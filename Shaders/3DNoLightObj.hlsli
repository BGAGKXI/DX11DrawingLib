//--------------------------------------------
//	テクスチャ
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
Texture2D SubTexture : register(t1);

SamplerState DecalSampler : register(s0);



//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 matWVP;
};

cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4	AmbientColor;	//環境光
	float4  EyePos;			//カメラ座標
};

static const float3 fogColor = { 0.8,0.8,0.6 };
static const float fogNear = 20.0;
static const float fogFar = 100.0;


//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//頂点カラー無し
};



//バーテックスシェーダー出力構造体
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wNormal : TEXCOORD1;	//ワールド法線
	float3 wPosition : TEXCOORD2;	//ワールド座標
	float2 Tex : TEXCOORD3;
};

//--------------------------------------------
//	拡散反射関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率(0～1.0)

//ランバートシェーディング
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// 負の値を０にする
	return K * C * D;
}
//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S*K*C;
	return S;
}
