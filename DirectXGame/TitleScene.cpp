#include "TitleScene.h"
#include "TextureManager.h"

#include <Input.h>



void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	titleTextureHandle_ = TextureManager::Load("title/title.png");
	sprite_ = Sprite::Create(titleTextureHandle_, {0, 0});

	audio_ = Audio::GetInstance();
	BGMHandle_ = audio_->LoadWave("mokugyo.wav");
}

void TitleScene::Update() {

	// BGMが再生されていない場合のみ再生する
	if (!isBGMPlaying_) {
		audio_->PlayAudio(BGMAudio_, BGMHandle_, true, 0.3f);
		isBGMPlaying_ = true; // フラグを立てる
	}

	if (Input::GetInstance()->PushKey(DIK_1)) {
		finished_ = true;
		audio_->StopAudio(BGMAudio_);
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
