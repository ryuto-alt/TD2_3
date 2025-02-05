#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "skydome.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameClearScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameClearScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameClearScene();

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

	bool IsFinished() const { return finished_; }

private: // メンバ変数
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// モデル
	Model* model_ = nullptr;
	Model* model2_ = nullptr;

	// SkyDome
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
