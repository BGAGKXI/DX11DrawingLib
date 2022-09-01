#include "toGbuffer.hlsli"

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
PSOutput main(PSInput input)
{
	PSOutput Out = (PSOutput)0;

	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
	float3 N = normalize(input.wNormal);  //ワールド法線
	float3 P = input.wPosition;				//ワールド座標
	Out.Color = color;
	Out.wNormal = float4(N, 1.0f);
	Out.wPosition = float4(P, 1.0f);

	return Out;
}
