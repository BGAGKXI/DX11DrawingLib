#include "flatShader.hlsli"
#include "Func.hlsli"
//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(PSInput input) : SV_TARGET
{
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
	float3 N = normalize(input.wNormal);
	float3 E = normalize(EyePos.xyz - input.wPos);
	float3 L = normalize(LightDir.rgb);

	// 環境光
	float3 A = AmbientColor.rgb;

	// 拡散反射
	float3 Kd = float3(1, 1, 1);
	float3 C = LightColor.rgb;
	float3 D = Diffuse(N, L, C, Kd);

	// 鏡面反射
	float3 Ks = float3(1, 1, 1);
	float3 S = PhongSpecular(N, L, C, E, Ks, 20);

	color *= input.Color * float4(A + D + S, 1.0f);

	return color;
}
