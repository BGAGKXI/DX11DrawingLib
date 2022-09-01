#include "toGbuffer_normalMap.hlsli"
//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
PSOutput main(PSInput input)
{
	PSOutput Out = (PSOutput)0;
	//接空間軸
	float3 vx = normalize(input.vTan);
	float3 vy = normalize(input.vBin);
	float3 vz = normalize(input.vNor);

	// 接空間行列
	float3x3 vMat = { { vx },{ vy },{ vz } };

	//　法線マップから法線取得
	float3 N = NormalTexture.Sample(DecalSampler, input.Tex).xyz;
	N = N * 2.0f - 1.0f;

	//　接空間からワールドへ変換(逆変換)
	N = normalize(mul(N, vMat));

	//テクスチャカラー
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);

	float3 P = input.wPos;
	Out.Color = color;
	Out.wNormal = float4(N, 1.0f);
	Out.wPosition = float4(P, 1.0f);

	return Out;
}
