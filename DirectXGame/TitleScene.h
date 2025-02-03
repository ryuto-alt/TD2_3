#pragma once

#include "Sprite.h"
#include "Input.h"
#include "Model.h"
#include "skydome.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"

class TitleScene {

public:
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }

private:
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;

	// 終了フラグ
	bool finished_ = false;
	Input* input_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	
	// モデル
	Model* model_ = nullptr;
	Model* model2_ = nullptr;

	// SkyDome
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;
};
