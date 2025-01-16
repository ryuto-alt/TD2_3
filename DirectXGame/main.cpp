#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameClear.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "GameScene10.h"
#include "GameScene2.h"
#include "GameScene3.h"
#include "GameScene4.h"
#include "GameScene5.h"
#include "GameScene6.h"
#include "GameScene7.h"
#include "GameScene8.h"
#include "GameScene9.h"
#include "TitleScene.h"

#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
GameOverScene* gameOverScene = nullptr;
GameClearScene* gameClearScene = nullptr;
GameScene2* gameScene2 = nullptr;
GameScene3* gameScene3 = nullptr;
GameScene4* gameScene4 = nullptr;
GameScene5* gameScene5 = nullptr;
GameScene6* gameScene6 = nullptr;
GameScene7* gameScene7 = nullptr;
GameScene8* gameScene8 = nullptr;
GameScene9* gameScene9 = nullptr;
GameScene10* gameScene10 = nullptr;


// 現在シーン
Scene scene = Scene::kUnkown;

const char* GetSceneName(Scene scenes) {
	switch (scenes) {
	case Scene::kTitle:
		return "Title";
	case Scene::kGame:
		return "Game";
	case Scene::kGame2:
		return "Game2";
	case Scene::kGame3:
		return "Game3";
	case Scene::kGame4:
		return "Game4";
	case Scene::kGame5:
		return "Game5";
	case Scene::kGame6:
		return "Game6";
	case Scene::kGame7:
		return "Game7";
	case Scene::kGame8:
		return "Game8";
	case Scene::kGame9:
		return "Game9";
	case Scene::kGame10:
		return "Game10";
	case Scene::kGameOver:
		return "Game Over";
	case Scene::kGameClear:
		return "Game Clear";
	default:
		return "Unknown";
	}
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kGame2;
			delete gameScene;
			gameScene = nullptr;
			gameScene2 = new GameScene2;
			gameScene2->Initialize();
		}
		break;
	case Scene::kGame2:
		if (gameScene2->IsFinished()) {
			scene = Scene::kGame3;
			delete gameScene2;
			gameScene2 = nullptr;
			gameScene3 = new GameScene3;
			gameScene3->Initialize();
		}
		break;
	case Scene::kGame3:
		if (gameScene3->IsFinished()) {
			scene = Scene::kGame4;
			delete gameScene3;
			gameScene3 = nullptr;
			gameScene4 = new GameScene4;
			gameScene4->Initialize();
		}
		break;
	case Scene::kGame4:
		if (gameScene4->IsFinished()) {
			scene = Scene::kGame5;
			delete gameScene4;
			gameScene4 = nullptr;
			gameScene5 = new GameScene5;
			gameScene5->Initialize();
		}
		break;
	case Scene::kGame5:
		if (gameScene5->IsFinished()) {
			scene = Scene::kGame6;
			delete gameScene5;
			gameScene5 = nullptr;
			gameScene6 = new GameScene6;
			gameScene6->Initialize();
		}
		break;
	case Scene::kGame6:
		if (gameScene6->IsFinished()) {
			scene = Scene::kGame7;
			delete gameScene6;
			gameScene6 = nullptr;
			gameScene7 = new GameScene7;
			gameScene7->Initialize();
		}
		break;
	case Scene::kGame7:
		if (gameScene7->IsFinished()) {
			scene = Scene::kGame8;
			delete gameScene7;
			gameScene7 = nullptr;
			gameScene8 = new GameScene8;
			gameScene8->Initialize();
		}
		break;
	case Scene::kGame8:
		if (gameScene8->IsFinished()) {
			scene = Scene::kGame9;
			delete gameScene8;
			gameScene8 = nullptr;
			gameScene9 = new GameScene9;
			gameScene9->Initialize();
		}
		break;
	case Scene::kGame9:
		if (gameScene9->IsFinished()) {
			scene = Scene::kGame10;
			delete gameScene9;
			gameScene9 = nullptr;
			gameScene10 = new GameScene10;
			gameScene10->Initialize();
		}
		break;
	case Scene::kGame10:
		if (gameScene10->IsFinished()) {
			scene = Scene::kGameOver;
			delete gameScene10;
			gameScene10 = nullptr;
			gameOverScene = new GameOverScene;
			gameOverScene->Initialize();
		}
		break;
	case Scene::kGameOver:
		if (gameOverScene->IsFinished()) {
			scene = Scene::kGameClear;
			delete gameOverScene;
			gameOverScene = nullptr;
			gameClearScene = new GameClearScene;
			gameClearScene->Initialize();
		}
		break;
	case Scene::kGameClear:
		if (gameClearScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameClearScene;
			gameClearScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kGame2:
		gameScene2->Update();
		break;
	case Scene::kGame3:
		gameScene3->Update();
		break;
	case Scene::kGame4:
		gameScene4->Update();
		break;
	case Scene::kGame5:
		gameScene5->Update();
		break;
	case Scene::kGame6:
		gameScene6->Update();
		break;
	case Scene::kGame7:
		gameScene7->Update();
		break;
	case Scene::kGame8:
		gameScene8->Update();
		break;
	case Scene::kGame9:
		gameScene9->Update();
		break;
	case Scene::kGame10:
		gameScene10->Update();
		break;
	case Scene::kGameOver:
		gameOverScene->Update();
		break;
	case Scene::kGameClear:
		gameClearScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kGame2:
		gameScene2->Draw();
		break;
	case Scene::kGame3:
		gameScene3->Draw();
		break;
	case Scene::kGame4:
		gameScene4->Draw();
		break;
	case Scene::kGame5:
		gameScene5->Draw();
		break;
	case Scene::kGame6:
		gameScene6->Draw();
		break;
	case Scene::kGame7:
		gameScene7->Draw();
		break;
	case Scene::kGame8:
		gameScene8->Draw();
		break;
	case Scene::kGame9:
		gameScene9->Draw();
		break;
	case Scene::kGame10:
		gameScene10->Draw();
		break;
	case Scene::kGameOver:
		gameOverScene->Draw();
		break;
	case Scene::kGameClear:
		gameClearScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	input = Input::GetInstance();
	input->Initialize();

	audio = Audio::GetInstance();
	audio->Initialize();

	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);
	Model::StaticInitialize();

	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// タイトルシーン初期化
	titleScene = new TitleScene;
	titleScene->Initialize();
	scene = Scene::kTitle;

	// メインループ
	while (true) {
		if (win->ProcessMessage()) {
			break;
		}

		imguiManager->Begin();
		input->Update();
		ChangeScene();
		UpdateScene();

		ImGui::Begin("Scene");
		ImGui::Text("Scene: %s", GetSceneName(scene));
		ImGui::End();

		axisIndicator->Update();
		imguiManager->End();

		dxCommon->PreDraw();
		DrawScene();
		axisIndicator->Draw();
		primitiveDrawer->Reset();
		imguiManager->Draw();
		dxCommon->PostDraw();
	}

	// 各種解放
	delete titleScene;
	delete gameScene;
	delete gameScene2;
	delete gameScene3;
	delete gameScene4;
	delete gameScene5;
	delete gameScene6;
	delete gameScene7;
	delete gameScene8;
	delete gameScene9;
	delete gameScene10;
	delete gameOverScene;
	delete gameClearScene;

	Model::StaticFinalize();
	audio->Finalize();
	imguiManager->Finalize();
	win->TerminateGameWindow();

	return 0;
}