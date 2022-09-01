#include "CircleShadow.h"


CIRCLESHADOW CircleShadow::circleShadow[CIRCLEMAX] = {};
void CircleShadow::SetCircleShadow(int index, XMFLOAT3 pos, float inner,float range)
{
	if (index < 0) return;
	if (index >= CIRCLEMAX)return;

	circleShadow[index].index = (float)index;
	circleShadow[index].inner = inner;
	circleShadow[index].pos.x = pos.x;
	circleShadow[index].pos.y = pos.y + 1.0f;
	circleShadow[index].pos.z = pos.z;
	circleShadow[index].pos.w = .0f;

	circleShadow[index].type = 1.0f;
	circleShadow[index].range = range;

}