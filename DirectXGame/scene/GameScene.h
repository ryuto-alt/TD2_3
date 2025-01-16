#pragma once

#include "AABB.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "mapchip.h"
#include <Camera.h>
#include <imgui.h>
#include "ImGuiManager.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void GenerateBlcoks();

	bool IsFinished() const { return finished_; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;

	// 終了フラグ
	bool finished_ = false;

	// 縦横ブロック配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	// マップチップフィールド
	MapChipField* mapChipField_;

	Model* NormalBlock = nullptr;
	Model* BomBlock = nullptr;
	Model* SlimeBlock = nullptr;
	Model* EnemyBlock = nullptr;
	Model* GoalBlock = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	/// <summary>
	/// ゲームシーン用
	/// </summary>

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
};
