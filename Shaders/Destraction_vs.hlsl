#include	"Destraction.hlsli"
//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------
GSInput main(VSInput input)
{
	//	�W�I���g���V�F�[�_�[���ō��W�ϊ�����̂�
	//���_�V�F�[�_�[�͂��̂܂ܑ���
	GSInput	output = (GSInput)0;
	output.Position = input.Position;
	output.Normal = input.Normal;
	output.Tex = input.Tex;
	output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return	output;
}