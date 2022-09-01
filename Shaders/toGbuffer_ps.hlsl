#include "toGbuffer.hlsli"

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
PSOutput main(PSInput input)
{
	PSOutput Out = (PSOutput)0;

	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);
	float3 N = normalize(input.wNormal);  //���[���h�@��
	float3 P = input.wPosition;				//���[���h���W
	Out.Color = color;
	Out.wNormal = float4(N, 1.0f);
	Out.wPosition = float4(P, 1.0f);

	return Out;
}
