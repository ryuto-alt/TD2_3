#pragma once
#include "AABB.h"
#include "Input.h"
#include "MathUtilityForText.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <numbers>

class MapChipField;
enum class LRDirection1 { kRight, kLeft };
enum Corner1 { kRightBottom1, kLeftBottom1, kRightTop1, kLeftTop1, kNumCorner1 };

struct CollisionMapInfo1 {
	bool hitCeilingFlag = false;
	bool landingFlag = false;
	bool wallContactFlag = false;
	Vector3 movement;
};

class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& pos);
	void Update();
	void Draw();

	void MovePlayer();

	void UpdateCenter();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3& GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3& velocity);

	void SetVelocityLeft(const Vector3& velocity);

	void CeilingContact(const CollisionMapInfo1& info);

	void InitializePosition(const Vector3& position);

	void GraundSetting(const CollisionMapInfo1& info);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void CheckMapCollision(CollisionMapInfo1& info);
	void CheckMapCollisionUp(CollisionMapInfo1& info, bool& hit);
	void CheckMapCollisionDown(CollisionMapInfo1& info, bool& hit);
	void CheckMapCollisionLeft(CollisionMapInfo1& info, bool& hit);
	void CheckMapCollisionRight(CollisionMapInfo1& info, bool& hit);

	Vector3 GetWorldPosition();
	AABB GetAABB();

	float GetRadius() const;

	void SetWorldPosition(const Vector3& position);

	LRDirection1 GetDirection() const { return lrdDirection_; }

	Vector3 CornerPosition(const Vector3& center, Corner1 corner);

	void JudgmentMove(const CollisionMapInfo1& info);

	void SetSnapEnabled(bool enabled) { snapEnabled_ = enabled; }

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 18.0f;

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビューポート
	ViewProjection* viewProjection_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	Vector3 velocity_ = {};

	LRDirection1 lrdDirection_ = LRDirection1::kRight;
	// 　マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	// 設置状態フラグ
	bool onGround_ = true;

	float radius_;

	bool snapEnabled_ = true; // デフォルトではスナップ処理を有効にする

	static inline const float kAttenuationLanding = 0.7f;
};
