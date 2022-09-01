#include "gaussianFilter.hlsli"
//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInput input) : SV_TARGET0
{
#if	0
	//�V�F�[�_�[���Ńe�N�X�`���T�C�Y���擾������@
	uint	width,	height;
	DiffuseTexture.GetDimensions( width, height );

	const	float2	texcel	= float2( 4.0f / (float)width, 4.0f / (float)height );
#endif

	float4 color = (float4)0;
	color.a = input.Color.a;
	for (int i = 0 ; i<KarnelSize * KarnelSize; i++) {
		float2 offset = texcel * Weight[i].xy;			
		float weight = Weight[i].z;
		color.rgb += DiffuseTexture.Sample(DecalSampler,input.Tex + offset).rgb * weight;
	}
	return color;
}

