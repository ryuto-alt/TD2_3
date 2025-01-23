#include "TitleScene.h"
#include "TextureManager.h"

#include <Input.h>



void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	titleTextureHandle_ = TextureManager::Load("title/title.png");
	sprite_ = Sprite::Create(titleTextureHandle_, {0, 0});

	audio_ = Audio::GetInstance();
	BGMHandle_ = audio_->LoadWave("mokugyo.wav");

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {

	// BGMが再生されていない場合のみ再生する
	if (!isBGMPlaying_) {
		audio_->PlayAudio(BGMAudio_, BGMHandle_, true, 0.3f);
		isBGMPlaying_ = true; // フラグを立てる
	}

	if (Input::GetInstance()->PushKey(DIK_1)) {
		fade_->Start(Fade::Status::FadeOut, 1.0f);
		phaseFade_ = PhaseFade::kFadoOut;
	}

	switch (phaseFade_) {

	case TitleScene::PhaseFade::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			phaseFade_ = PhaseFade::kMain;
		}
		break;

	case TitleScene::PhaseFade::kFadoOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			//fade_->Stop();
			audio_->StopAudio(BGMAudio_);
			finished_ = true;
		}
		break;
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
	
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
