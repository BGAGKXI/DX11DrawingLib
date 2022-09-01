#include	"Destraction.hlsli"

float	rand(float2 seed)
{
	//	このマジックナンバーに特別な意味はありません。テキトーです。
	return	frac(sin(dot(seed, float2(54.35676f, 78.35676f))) * 43758.5453123f);
}

matrix	rotation(float3 angle)
{
	float sx, sy, sz, cx, cy, cz;
	sincos(angle.x, sx, cx);
	sincos(angle.y, sy, cy);
	sincos(angle.z, sz, cz);
	matrix	rot;
	rot._11 = cy * cz;
	rot._12 = cy * sz;
	rot._13 = -sy;
	rot._14 = 0;
	rot._21 = sx * sy * cz - cx * sz;
	rot._22 = sx * sy * sz + cx * cz;
	rot._23 = sx * cy;
	rot._24 = 0;
	rot._31 = cx * sy * cz + sx * sz;
	rot._32 = cx * sy * sz - sx * cz;
	rot._33 = cx * cy;
	rot._34 = 0;
	rot._41 = 0;
	rot._42 = 0;
	rot._43 = 0;
	rot._44 = 1;
	return rot;
}
float3 rotation(float3 v, float3 angle)
{
	float3 ret = mul(float4(v, 0), rotation(angle)).xyz;
	return ret;
}


//--------------------------------------------
//	ジオメトリシェーダー
//--------------------------------------------
[maxvertexcount(3)]
void	main(triangle GSInput input[3], inout TriangleStream<PSInput> output)
{
	//	ポリゴンの重心を算出
	float3 center = (input[0].Position.xyz + input[1].Position.xyz + input[2].Position.xyz) / 3.0f;

	//	面法線を算出
	float3 normal = cross(normalize(input[1].Position.xyz - input[0].Position.xyz),
		normalize(input[2].Position.xyz - input[0].Position.xyz));

	//	乱数ベクトルを生成
	float randomValue = 2.0f * (rand(center.xy) - 0.5f);
	float3 randomVector = randomValue.xxx;

	//	破壊パラメーター
	float destraction = Parameter.x;
	float scalingFactor = Parameter.y;
	float rotationFactor = Parameter.z;
	float translationFactor = Parameter.w;
	//	面を作成
	for (int i = 0; i < 3; ++i)
	{
		PSInput element;
		element.Position = float4(input[i].Position, 1);
		element.wNormal = mul(World, float4(input[i].Normal, 0)).xyz;
		element.Color = input[i].Color;
		element.Tex = input[i].Tex;

		//	中心点を起点に拡縮処理
		element.Position.xyz = (element.Position.xyz - center) * (1.0f - scalingFactor) + center;
		//	中心点を起点に回転処理
		element.Position.xyz = rotation(element.Position.xyz - center, randomVector * rotationFactor) + center;
		//	面法線方向に位置を変化
		element.Position.xyz += normal * translationFactor * randomVector;
		element.wPosition = element.Position.xyz;

		element.Position = mul(matWVP, element.Position);

		//	ここで頂点を生成している
		output.Append(element);
	}

	//	追加した頂点でプリミティブを構成し、新しいプリミティブを生成する
	output.RestartStrip();
}
