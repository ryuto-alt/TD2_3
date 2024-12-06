#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "GameClear.h"

#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
GameOverScene* gameOverScene = nullptr;
GameClearScene* gameClearScene = nullptr;

enum class Scene {
	kUnkown = 0,
	kTitle,
	kGame,
	kGameOver,
	kGameClear

};
// 現在シーン
Scene scene = Scene::kUnkown;

const char* GetSceneName(Scene scenes) {
	switch (scenes) {
	case Scene::kTitle:
		return "Title";
	case Scene::kGame:
		return "Game";
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
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGameOver;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			gameOverScene = new GameOverScene;
			gameOverScene->Initialize();
		}
		break;
	case Scene::kGameOver:
		if (gameOverScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGameClear;
			// 旧シーンの解放
			delete gameOverScene;
			gameOverScene = nullptr;
			// 新シーンの生成と初期化
			gameClearScene = new GameClearScene;
			gameClearScene->Initialize();
		}
		break;
	case Scene::kGameClear:
		if (gameClearScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameClearScene;
			gameClearScene = nullptr;
			// 新シーンの生成と初期化
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
	// 汎用機能
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
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// タイトルシーン初期化
	titleScene = new TitleScene;
	titleScene->Initialize();
	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
	scene = Scene::kGameOver;
	//ゲームオーバーシーン初期化
	gameOverScene = new GameOverScene();
	gameOverScene->Initialize();
	scene = Scene::kGameClear;
	//ゲームクリアシーン初期化
	gameClearScene = new GameClearScene();
	gameClearScene->Initialize();
	scene = Scene::kTitle;

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		//シーン
		ChangeScene();
		UpdateScene();

		
		//imGui
		ImGui::Begin("Scene");
		ImGui::Text("Scene: %s", GetSceneName(scene)); // シーン名を表示
		ImGui::End();
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();
		// 描画開始
		dxCommon->PreDraw();

		DrawScene();

		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	delete titleScene;
	delete gameScene;
	delete gameOverScene;
	delete gameClearScene;

	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}
