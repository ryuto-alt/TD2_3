#define NOMINMAX
#include "Enemy.h"
#include "Input.h"
#include "mapchip.h"
#include <DebugText.h>
#include <algorithm>
#include <array>
#include <cassert>
#ifdef _DEBUG
#include "imgui.h"
#endif

#include <iostream>
#include <mymath.h>
#include <numbers>

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	model_ = model;

	viewProjection_ = viewProjection;
}

void Enemy::Update() {

	worldTransform_.TransferMatrix();

	MovePlayer();

	// 衝突情報を初期化
	CollisionMapInfo1 collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.movement = velocity_;
	collisionMapInfo.landingFlag = false;
	collisionMapInfo.wallContactFlag = false;
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	JudgmentMove(collisionMapInfo);

	CeilingContact(collisionMapInfo);

	GraundSetting(collisionMapInfo);

	worldTransform_.UpdateMatarix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

void Enemy::MovePlayer() {
	// 右移動操作
	if (Input::GetInstance()->PushKey(DIK_D)) {
		velocity_.x = MapChipField::kBlockWidth / 4;
		lrdDirection_ = LRDirection1::kRight;
	}
	// 左移動操作
	else if (Input::GetInstance()->PushKey(DIK_A)) {
		velocity_.x = -MapChipField::kBlockWidth / 4;
		lrdDirection_ = LRDirection1::kLeft;
	}
	// どちらのキーも押されていない場合
	else {
		velocity_.x = 0;
	}
}


void Enemy::UpdateCenter() {
	worldTransform_.TransferMatrix();

	// X方向の速度はリセットせずそのまま
	velocity_.x = velocity_.x;
	velocity_.y = 0;

	CollisionMapInfo1 collisionMapInfo;
	collisionMapInfo.movement = velocity_;
	collisionMapInfo.landingFlag = false;
	collisionMapInfo.wallContactFlag = false;

	CheckMapCollision(collisionMapInfo);
	JudgmentMove(collisionMapInfo);
	CeilingContact(collisionMapInfo);
	GraundSetting(collisionMapInfo);

	worldTransform_.UpdateMatarix();
}

void Enemy::SetVelocity(const Vector3& velocity) {
	velocity_ = velocity;
	worldTransform_.translation_ += velocity_; // 速度を設定した際に即座に位置を更新
}

void Enemy::CeilingContact(const CollisionMapInfo1& info) {
	// 天井、当り判定
	if (info.hitCeilingFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Enemy::GraundSetting(const CollisionMapInfo1& info) {
	// 接地状態の切り替え処理
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態の移行
			onGround_ = false;
		} else {
			std::array<Vector3, kNumCorner1> positonsNew;

			for (uint32_t i = 0; i < positonsNew.size(); ++i) {
				positonsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner1>(i));
			}

			MapChipType mapChipType;
			bool hit = false;

			// 左上点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftTop1] + Vector3(0, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::lBlock) {
				hit = true;
			}

			// 右上点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightTop1] + Vector3(0, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::lBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 着地
		if (info.hitCeilingFlag) {
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 上方向速度をリセット
			velocity_.y = 0.0f;
			// 着地状態に切り替える
			onGround_ = true;
		}
	}
}

void Enemy::CheckMapCollision(CollisionMapInfo1& info) {
	bool hit = false;

	// 上方向の衝突判定
	CheckMapCollisionUp(info, hit);
	// 下方向の衝突判定
	CheckMapCollisionDown(info, hit);
	// 左方向の衝突判定
	CheckMapCollisionLeft(info, hit);
	// 右方向の衝突判定
	CheckMapCollisionRight(info, hit);
	// 衝突時の移動量と速度をゼロに
	if (hit) {
		info.movement = Vector3(0, 0, 0);
		velocity_ = Vector3(0, 0, 0);
	}
}

void Enemy::CheckMapCollisionUp(CollisionMapInfo1& info, bool& hit) {
	if (info.movement.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner1> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner1>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::lBlock && mapChipTypeNext != MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::lBlock && mapChipTypeNext != MapChipType::lBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 3.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.movement + Vector3(0, +kHeight / 3.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 3.0f + kBlank));
			info.hitCeilingFlag = true;
		}
	}
}

void Enemy::CheckMapCollisionDown(CollisionMapInfo1& info, bool& hit) {
	if (info.movement.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner1> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(0, info.movement.y, 0), static_cast<Corner1>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::lBlock && mapChipTypeNext != MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::lBlock && mapChipTypeNext != MapChipType::lBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.movement + Vector3(0, -kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + ((kHeight / 2.0f) + kBlank));
			info.landingFlag = true;
		}
	}
}

void Enemy::CheckMapCollisionLeft(CollisionMapInfo1& info, bool& hit) {
	std::array<Vector3, kNumCorner1> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(info.movement.x, 0, 0), static_cast<Corner1>(i));
	}

	MapChipType mapChipType;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = std::min(0.0f, (rect.left - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank));
	}
}

void Enemy::CheckMapCollisionRight(CollisionMapInfo1& info, bool& hit) {
	std::array<Vector3, kNumCorner1> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(info.movement.x, 0, 0), static_cast<Corner1>(i));
	}

	MapChipType mapChipType;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop1]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = std::max(0.0f, (rect.right - worldTransform_.translation_.x) - (kWidth / 2.0f));
	}
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - radius_, worldPos.y - radius_, worldPos.z - radius_};
	aabb.max = {worldPos.x + radius_, worldPos.y + radius_, worldPos.z + radius_};
	return aabb;

	return aabb;
}

float Enemy::GetRadius() const { return radius_; }


void Enemy::SetWorldPosition(const Vector3& position) {
	worldTransform_.translation_ = position;
	worldTransform_.UpdateMatarix();
}

Vector3 Enemy::CornerPosition(const Vector3& center, Corner1 corner) {

	Vector3 offsetTable[kNumCorner1] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Enemy::JudgmentMove(const CollisionMapInfo1& info) {
	// 移動
	worldTransform_.translation_ += info.movement;
}
