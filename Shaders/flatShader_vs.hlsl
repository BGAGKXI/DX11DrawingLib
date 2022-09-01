#include "flatShader.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	float4 P = float4(input.Position, 1.0);

	// ワールド変換座標
	float4 worldPos = mul(World, P);
	output.Position = P;
	output.Color = float4(1, 1, 1, 1);
	output.Tex = input.Tex;
	output.wPos = worldPos.xyz;

	return output;
}
