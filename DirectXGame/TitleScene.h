#pragma once
#include "Sprite.h"
#include "Input.h"
#include "Audio.h"
#include "Fade.h"
#include "DirectXCommon.h"
class TitleScene {

public:
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }

private:
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

	// BGM
	Audio* audio_ = nullptr;
	int BGMHandle_ = 0;
	int BGMAudio_ = -1;
	// BGMが再生されているかを追跡
	bool isBGMPlaying_ = false;

	// 終了フラグ
	bool finished_ = false;
	Input* input_ = nullptr;

	//スプライト
	Sprite* sprite_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
};