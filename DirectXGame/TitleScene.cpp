#include "TitleScene.h"
#include "TextureManager.h"

#include <Input.h>



void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	titleTextureHandle_ = TextureManager::Load("title/title.png");
	sprite_ = Sprite::Create(titleTextureHandle_, {0, 0});
}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_1)) {
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
