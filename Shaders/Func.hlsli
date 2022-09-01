//--------------------------------------------
//	拡散反射関数(ランバート)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// 負の値を０にする
	return C * D * K;
}

//--------------------------------------------
//	ハーフランバート
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 HalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = D * 0.5 + 0.5;
	//D = D * D;	// いい感じに減衰する
	return C * D * K;
}





//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}
//--------------------------------------------
//	鏡面反射関数(フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//反射ベクトル
	float3 R = normalize(reflect(L,N));
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}
//--------------------------------------------
// リムライト
//--------------------------------------------
// N:法線(正規化済み)
// E:視点方向ベクトル(正規化済み)
// L:入射ベクトル(正規化済み)
// LightColor :ライト色
float3 RimLight(
	float3 N, float3 E, float3 L, float3 LightColor)
{
	float rim = 1.0 - max(dot(N, E), 0.0);
	float ratio = max(dot(L, E), 0);
	return  LightColor * rim * ratio;
}


//--------------------------------------------
// 半球ライト
//--------------------------------------------
// N:法線(正規化済み)
// UP:上方向（片方）
// SkyColor :空(上)色
// GroundColor :地面(下)色
float3 HemiSphereLight(float3 N, float3 UP, float3 SkyColor, float3 GroundColor)
{
	float skyblend = dot(N, UP);
	float groundblend = skyblend < 0.0 ? -skyblend : 0.0;
	skyblend = max(0, skyblend);
	return (SkyColor * skyblend + GroundColor * groundblend);
}

//(dot(normal, invSky) + 1.0) * 0.5;
//--------------------------------------------
//	フォグ
//--------------------------------------------
//color		:現在の色
//viewPos	:視点
//Pos		:ピクセルの位置
//fogColor	:フォグの色
//Near		:フォグの近距離
//Far		:フォグの遠距離
float3 Fog(float3 color, float3 viewPos, float3 Pos, float3 fogColor, 
float Near, float Far)
{
	float Len = length(viewPos - Pos);
	float fogalpha = saturate((Len - Near) / (Far - Near));
	return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}


//color		:現在の色
//viewPos	:視点
//Pos		:ピクセルの位置
//fogColor	:フォグの色
//Near		:フォグの近距離
//Far		:フォグの遠距離
float3 Fog2(float3 color, float3 Pos, float3 fogColor, float Low, float Heigh)
{
	float Len = Heigh - Pos.y;
	float fogalpha = saturate((Len) / (Heigh - Low));
	return color.rgb * (1.0 - fogalpha) + fogColor * fogalpha;
}

