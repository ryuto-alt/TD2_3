#pragma once

#include "AABB.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Enemy.h"
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

	// BGM
	int BGMHandle_ = 0;
	int BGMAudio_ = -1;
	// BGMが再生されているかを追跡
	bool isBGMPlaying_ = false;


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
	Player* playerBottom = new Player();
	Player* playerTop = new Player();
	Player* playerLeft = new Player();
	Player* playerRight = new Player();
	Player* playerCenter = new Player();

	Enemy* enemy_ = new Enemy();

	Camera* Camera_ = nullptr;

	Vector3 railcameraPos = {15.5f, 27.5f, -40.0f};
	Vector3 railcameraRad = {0, 0, 0};

	// huragu
	bool ShotPlayer = false;

	// SkyDome
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;
	// std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	float ChangeDelay = 10.0f;

	float radius_ = 1.5f;

	float offsetX = 0.0f;

	bool hitBottom = false;
	bool hitTop = false;
	bool hitLeft = false;
	bool hitRight = false;

	int Scene;

	// float triggerDistance = 1.0f; // 適切な値に設定
};
