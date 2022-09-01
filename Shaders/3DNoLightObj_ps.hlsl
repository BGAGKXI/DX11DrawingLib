#include "3DNoLightObj.hlsli"


//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(PSInput input) : SV_Target
{
	float4 color = DiffuseTexture.Sample(DecalSampler,input.Tex);
	return color;
}


