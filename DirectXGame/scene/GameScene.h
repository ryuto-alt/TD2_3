#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Fade.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include"mapchip.h"

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

	void GenerateBlocks();

	bool IsFinished() const { return finished_; }

private: // メンバ変数
	// シーンのフェード
	enum class PhaseFade {
		kFadeIn,	// フェードイン
		kMain,	    // メイン部
		kFadoOut,	// フェードアウト
	};
	// フェード
	Fade* fade_ = nullptr;
	// 現在のフェーズ
	PhaseFade phaseFade_ = PhaseFade::kFadeIn;

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

	//終了フラグ
	bool finished_ = false;

	// 縦横ブロック配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	// マップチップフィールド
	MapChipField* mapChipField_;
	
	Model*NormalBlock = nullptr;
	Model*BomBlock = nullptr;
	Model*SlimeBlock = nullptr;
	Model*EnemyBlock = nullptr;
	Model*GoalBlock = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
