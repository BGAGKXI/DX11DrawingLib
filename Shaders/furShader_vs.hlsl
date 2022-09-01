#include "furShader.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------

GSInput main(VSInput input)
{
	GSInput output = (GSInput)0;
	float4 P = float4(input.Position, 1.0);
	float3 N = normalize(input.Normal);
	output.Position = P;

	output.Tex = input.Tex;
	output.Normal = N;
	return output;
}
