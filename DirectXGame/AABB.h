#pragma once
#pragma once
#include <Vector3.h>

struct AABB {
	Vector3 min;
	Vector3 max;
};

bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2);
