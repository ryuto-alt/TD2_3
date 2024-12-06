#pragma once
#include "Sprite.h"
#include "Input.h"
#include "DirectXCommon.h"
class TitleScene {

public:
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }

private:
	// 終了フラグ
	bool finished_ = false;
	Input* input_ = nullptr;
	//スプライト
	Sprite* sprite_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	
};
