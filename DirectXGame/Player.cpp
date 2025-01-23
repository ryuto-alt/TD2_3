#define NOMINMAX
#include "Input.h"
#include "mapchip.h"
#include <DebugText.h>
#include <Player.h>
#include <algorithm>
#include <array>
#include <cassert>
#ifdef _DEBUG
#include "imgui.h"
#endif

#include <iostream>
#include <mymath.h>
#include <numbers>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
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

void Player::Update() {

	worldTransform_.TransferMatrix();

	MovePlayer();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
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

void Player::Update2() {

	worldTransform_.TransferMatrix();

	MovePlayer2();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
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
void Player::SetPosition(const Vector3& position) {
	worldTransform_.translation_ = position;
	worldTransform_.UpdateMatarix();
}
void Player::inPlayerUpdate() {
	worldTransform_.TransferMatrix();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.movement = velocity_;
	collisionMapInfo.landingFlag = false;
	collisionMapInfo.wallContactFlag = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	CeilingContact(collisionMapInfo);
	GraundSetting(collisionMapInfo);

	// 移動処理
	JudgmentMove(collisionMapInfo);

	worldTransform_.UpdateMatarix();
}

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_); }

// 当たり判定
Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - radius_, worldPos.y - radius_, worldPos.z - radius_};
	aabb.max = {worldPos.x + radius_, worldPos.y + radius_, worldPos.z + radius_};
	return aabb;

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	// ジャンプ開始
	velocity_ += Vector3(0, kJumpAcceleration / 1.0f, 0);
}

void Player::MovePlayer() {
	// 右移動操作
	if (Input::GetInstance()->PushKey(DIK_D) && worldTransform_.translation_.x < 28) {
		// 移動
		velocity_.x = MapChipField::kBlockWidth / 4;
	}
	// 左移動操作
	else if (Input::GetInstance()->PushKey(DIK_A) && worldTransform_.translation_.x > 2) {
		// 移動
		velocity_.x = -MapChipField::kBlockWidth / 4;
	}
	// どちらのキーも押されていない場合
	else {
		// 停止
		velocity_.x = 0;
		SnapToBlockX();
	}
}

void Player::MovePlayer2() {
	// 上移動操作
	if (Input::GetInstance()->PushKey(DIK_W) && worldTransform_.translation_.y < 36) {
		// 移動
		velocity_.y = MapChipField::kBlockHeight / 4;
	}
	// 下移動操作
	else if (Input::GetInstance()->PushKey(DIK_S) && worldTransform_.translation_.y > 18) {
		// 移動
		velocity_.y = -MapChipField::kBlockHeight / 4;
	}
	// どちらのキーも押されていない場合
	else {
		// 停止
		velocity_.y = 0;
		SnapToBlockY();
	}
}

void Player::SnapToBlockX() {
	// X座標をブロック幅にスナップ
	float snappedX = round(worldTransform_.translation_.x / MapChipField::kBlockWidth) * MapChipField::kBlockWidth;
	worldTransform_.translation_.x = snappedX;
}

void Player::SnapToBlockY() {
	// Y座標をブロック高さにスナップ
	float snappedY = round(worldTransform_.translation_.y / MapChipField::kBlockHeight) * MapChipField::kBlockHeight;
	worldTransform_.translation_.y = snappedY;
}

void Player::CeilingContact(const CollisionMapInfo& info) {
	// 天井、当り判定
	if (info.hitCeilingFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	bool hit = false;

	CheckMapCollisionUp(info, hit);
	CheckMapCollisionDown(info, hit);
	CheckMapCollisionRight(info, hit);
	CheckMapCollisionLeft(info, hit);

	if (hit) {
		info.movement = Vector3(0, 0, 0);
	}
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info, bool& hit) {
	if (info.movement.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::lBlock && mapChipTypeNext != MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
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

void Player::CheckMapCollisionDown(CollisionMapInfo& info, bool& hit) {
	if (info.movement.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(0, info.movement.y, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::lBlock && mapChipTypeNext != MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
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

void Player::CheckMapCollisionRight(CollisionMapInfo& info, bool& hit) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(info.movement.x, 0, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = std::max(0.0f, (rect.right - worldTransform_.translation_.x) - (kWidth / 2.0f));
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info, bool& hit) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(info.movement.x, 0, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::lBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = std::min(0.0f, (rect.left - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank));
	}
}

void Player::GraundSetting(const CollisionMapInfo& info) {
	// 接地状態の切り替え処理
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態の移行
			onGround_ = false;
		} else {
			std::array<Vector3, kNumCorner> positonsNew;

			for (uint32_t i = 0; i < positonsNew.size(); ++i) {
				positonsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			bool hit = false;

			// 左上点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftTop] + Vector3(0, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::lBlock) {
				hit = true;
			}

			// 右上点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightTop] + Vector3(0, 0, 0));
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

void Player::JudgmentMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.movement;
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
