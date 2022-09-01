#include	"ToonRendering.hlsli"
//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
GSInput main(VSInput input)
{
	//	ジオメトリシェーダー側で座標変換するので
	//頂点シェーダーはそのまま送る
	GSInput	output = (GSInput)0;
	output.Position = input.Position;
	output.Normal = input.Normal;
	output.Tex = input.Tex;
	output.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return	output;
}
