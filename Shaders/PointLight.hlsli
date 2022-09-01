
//--------------------------------------------
//	ポイントライト構造体
//--------------------------------------------

struct POINTLIGHT {
	float index;
	float range;//光の届く範囲
	float type; //有効か無効か
	float dummy;
	float4 pos;
	float4 color;
};

//--------------------------------------------
//	コンスタントバッファ
//--------------------------------------------
static const int POINTMAX = 32;

cbuffer CBPerFrame : register(b3)
{
	POINTLIGHT PointLight[POINTMAX];
};
//--------------------------------------------
//	ポイントライト関数
//--------------------------------------------

// P: ピクセルワールド位置
// N: ワールド法線(正規化済み)
// E: 視線方向ベクトル(正規化済み)
// Kd: 拡散反射率
// Ks: 鏡面反射率 

float3 AddPointLight(float3 P, float3 N, float3 E, float3 Kd, float3 Ks)
{
	float3 PL;				//ポイントライトベクトル
	float3 PC;				//ポイントライト色
	float influence;		//ポイントライトの影響
	float3 PD = (float3)0;	//拡散反射
	float3 PS = (float3)0;	//鏡面反射
	for (int i = 0; i < POINTMAX; i++) {
		//点灯していないライト除外
		if (PointLight[i].type == 0) continue;
		PL = P - PointLight[i].pos.xyz;
		float d = length(PL);
		float r = PointLight[i].range;
		//届かないライト除外
		if (d > r) continue;
		influence = saturate(1.0f - d / r);
		PL = normalize(PL);
		PC = PointLight[i].color.rgb;
		//拡散光の加算		
		PD += Diffuse(N, PL, PC, Kd)*influence;
		//鏡面光の加算
		PS += PhongSpecular(N, PL, PC, E, Ks, 20)*influence;
	}
	return (PD + PS);
}

