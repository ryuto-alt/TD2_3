#pragma once
#include "Sprite.h"
#include "Input.h"
#include "DirectXCommon.h"

enum class Scene {
	kUnkown = 0,
	kTitle, 
	kGame,
	kGame2, 
	kGame3,
	kGame4,
	kGame5, 
	kGame6, 
	kGame7, 
	kGame8,
	kGame9,
	kGame10,
	kGameOver, 
	kGameClear 
};


class TitleScene {

public:
	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }

	Scene GetNextScene() const { return nextScene_; } // 次のシーンを取得

private:
	// 終了フラグ
	bool finished_ = false;
	Input* input_ = nullptr;
	//スプライト
	Sprite* sprite_ = nullptr;
	Scene nextScene_ = Scene::kUnkown; // 次のシーンを保持
	DirectXCommon* dxCommon_ = nullptr;

	uint32_t titleTextureHandle_ = 0;
	
};
