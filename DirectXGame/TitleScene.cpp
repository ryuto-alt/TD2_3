#include "TitleScene.h"
#include "TextureManager.h"

#include <Input.h>



// Scene列挙型を定義

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	titleTextureHandle_ = TextureManager::Load("title/title.png");
	sprite_ = Sprite::Create(titleTextureHandle_, {0, 0});
}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}


	 // キーボードの数字キーでシーン遷移
	if (Input::GetInstance()->PushKey(DIK_1)) {
		nextScene_ = Scene::kGame; // GameScene1に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_2)) {
		nextScene_ = Scene::kGame2; // GameScene2に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_3)) {
		nextScene_ = Scene::kGame3; // GameScene3に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_4)) {
		nextScene_ = Scene::kGame4; // GameScene4に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_5)) {
		nextScene_ = Scene::kGame5; // GameScene5に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_6)) {
		nextScene_ = Scene::kGame6; // GameScene6に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_7)) {
		nextScene_ = Scene::kGame7; // GameScene7に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_8)) {
		nextScene_ = Scene::kGame8; // GameScene8に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_9)) {
		nextScene_ = Scene::kGame9; // GameScene9に遷移
		finished_ = true;
	}
	if (Input::GetInstance()->PushKey(DIK_0)) {
		nextScene_ = Scene::kGame10; // GameScene10に遷移
		finished_ = true;
	}




}

void TitleScene::Draw() { 
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	sprite_->Draw(); 
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
	

}
