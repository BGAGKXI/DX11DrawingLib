#include "toGbuffer_normalMap.hlsli"
//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
PSOutput main(PSInput input)
{
	PSOutput Out = (PSOutput)0;
	//�ڋ�Ԏ�
	float3 vx = normalize(input.vTan);
	float3 vy = normalize(input.vBin);
	float3 vz = normalize(input.vNor);

	// �ڋ�ԍs��
	float3x3 vMat = { { vx },{ vy },{ vz } };

	//�@�@���}�b�v����@���擾
	float3 N = NormalTexture.Sample(DecalSampler, input.Tex).xyz;
	N = N * 2.0f - 1.0f;

	//�@�ڋ�Ԃ��烏�[���h�֕ϊ�(�t�ϊ�)
	N = normalize(mul(N, vMat));

	//�e�N�X�`���J���[
	float4 color = DiffuseTexture.Sample(DecalSampler, input.Tex);

	float3 P = input.wPos;
	Out.Color = color;
	Out.wNormal = float4(N, 1.0f);
	Out.wPosition = float4(P, 1.0f);

	return Out;
}
