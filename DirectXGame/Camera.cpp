#include "Camera.h"

void Camera::Initialize(const Vector3& pos, const Vector3& rad) {

	worldtransfrom_.translation_ = pos;
	worldtransfrom_.rotation_ = rad;

	worldtransfrom_.Initialize();
	camera_.Initialize();
}

void Camera::Update() {

	// camera_.rotation_.y += 0.11f;
	// camera_.translation_.x+= 0.01f;
	// worldtransfrom_.rotation_.y -= 0.001f;

	worldtransfrom_.UpdateMatarix();
	camera_.UpdateMatrix();
	camera_.matView = Inverse(worldtransfrom_.matWorld_);
	camera_.TransferMatrix();
}
