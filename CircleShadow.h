#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct CIRCLESHADOW {
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float inner;
	XMFLOAT4 pos;
};

class CircleShadow
{
public:
	static const int CIRCLEMAX = 32;
	static CIRCLESHADOW circleShadow[CIRCLEMAX];
	static void SetCircleShadow(int index, XMFLOAT3 pos, float inner, float range);
};
