#pragma once
#include "ViewProjection.h"
#include <WorldTransform.h>
#include <mymath.h>

class Player;

class Camera {

public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void Initialize(const Vector3& pos, const Vector3& rad);
	void Update();

	Player* target_ = nullptr;

	void setTarget(Player* target) { target_ = target; }
	const ViewProjection& GetViewProjection() { return camera_; }
	const WorldTransform& GetWorldTransform() { return worldtransfrom_; }

private:
	WorldTransform worldtransfrom_;

	ViewProjection camera_;

	Vector3 velocity_ = {1.0f, 1.0f, 1.0f};
};
