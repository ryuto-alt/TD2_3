#pragma once

#include "AABB.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "Skydome.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "mapchip.h"

#include <Camera.h>
#include <imgui.h>

class GameScene5 {
public:
	GameScene5();
	~GameScene5();

	void Initialize();
	void Update();
	void Draw();
	void GenerateBlcoks();
	bool IsFinished() const { return finished_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;

	bool finished_ = false;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	MapChipField* mapChipField_;

	Model* NormalBlock = nullptr;
	Model* BomBlock = nullptr;
	Model* SlimeBlock = nullptr;
	Model* EnemyBlock = nullptr;
	Model* GoalBlock = nullptr;

	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	Model* modelPlayer_ = nullptr;
	std::list<Player*> players_;
	Player* player = new Player();
	Player* player2 = new Player();
	Player* player3 = new Player();
	Player* player4 = new Player();

	Camera* Camera_ = nullptr;

	Vector3 railcameraPos = {-85, 7, -40};
	Vector3 railcameraRad = {0, 0, 0};

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(8, 12);
	Vector3 playerPosition2 = mapChipField_->GetMapChipPositionByIndex(8, -1);
	Vector3 playerPosition3 = mapChipField_->GetMapChipPositionByIndex(-1, 5);
	Vector3 playerPosition4 = mapChipField_->GetMapChipPositionByIndex(16, 5);

	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;
};