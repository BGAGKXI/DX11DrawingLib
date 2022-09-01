#include "ToonRendering.hlsli"
//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(PSInput input):SV_TARGET0
{
	float4 C	= DiffuseTexture.Sample(DecalSampler, input.Tex)
							 * input.Color;

	float3 N = normalize(input.wNormal);
	float3 E = normalize(EyePos.xyz - input.wPosition);
	float3 L = normalize(LightDir.xyz);

	//	環境光
	float3 A = AmbientColor.rgb;

	//	拡散反射
	float3 D = (float3)0;
	float U = dot(-L, N) * 0.5f + 0.5f;
	D = RampTexture.Sample(RampSampler, float2(U, 1)).rgb;


	//	鏡面反射
	float3	S = (float3)0;
	U = dot(normalize(reflect(L, N)), E) * 0.5f + 0.5f;
	S = RampTexture.Sample(RampSampler, float2(U, 1)).rgb;
	S = pow(saturate(S), 20) * 0.5f;

	//	リムライト
	float3	R = (float3)0;
	float	Rim = 1.0f - max(dot(N, E), 0.0f);
	float	Ratio = max(dot(L, E), 0);
	R = RampTexture.Sample(RampSampler, float2(Rim * Ratio, 1)).r;
	//	ライティング
	C.rgb *= A + D + S + R;
	return	C;
}
