#pragma once
#include "AABB.h"
#include "Input.h"
#include "MathUtilityForText.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <numbers>

class MapChipField;
class Enemy;

enum class LRDirection { kRight, kLeft };

enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

struct CollisionMapInfo {
	bool hitCeilingFlag = false;
	bool landingFlag = false;
	bool wallContactFlag = false;
	Vector3 movement;
};

// 自キャラ
class Player {
public:
	// 初期化
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& pos);
	// 更新
	void Update();
	void Update2();
	void Update3();
	void Draw();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3& GetVelocity() { return velocity_; }
	void SetVelocity(const Vector3& velocity);                 // 追加
	static float GetLimitRunSpeed() { return kLimitRunSpeed; } // 追加

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void MovePlayer();
	void MovePlayer2();

	void SnapToBlockX();
	void SnapToBlockY();

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info, bool& hit);
	void CheckMapCollisionDown(CollisionMapInfo& info, bool& hit);
	void CheckMapCollisionLeft(CollisionMapInfo& info, bool& hit);
	void CheckMapCollisionRight(CollisionMapInfo& info, bool& hit);

	void JudgmentMove(const CollisionMapInfo& info);
	void CeilingContact(const CollisionMapInfo& info);
	void GraundSetting(const CollisionMapInfo& info);
	void UpdateCenter();
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Enemy* enemy);

	void Stop() {
		velocity_ = {0, 0, 0};
		SnapToBlockX();
		SnapToBlockY();
	} // 修正

	void SetWorldPosition(const Vector3& position);

	LRDirection GetDirection() const { return lrdDirection_; }

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kJumpAcceleration = 0.5f;
	static inline const float kGravityAcceleration = 0.07f;
	static inline const float kAttenuationWall = 0.2f;
	static inline const float kAttenuationLanding = 0.7f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kTimeTurn = 0.5f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 18.0f;
	static inline const float kGroundSearchHeight = 0.06f;

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビューポート
	ViewProjection* viewProjection_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	Vector3 velocity_ = {};

	LRDirection lrdDirection_ = LRDirection::kRight;

	// 　マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	// 設置状態フラグ
	bool onGround_ = true;

	Vector3 worldPos_;
	float radius_;


};
