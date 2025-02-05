#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "TitleScene.h"
// #include "GameClear.h"  // ゲームクリアシーンが不要ならコメントアウト（必要なら同様の修正する）

#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
GameOverScene* gameOverScene = nullptr;
// GameClearScene* gameClearScene = nullptr; // 不要なら削除

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kGameOver
	// kGameClear  // ゲームクリアシーン使うなら追加
};

// 現在シーン
Scene scene = Scene::kUnknown;

const char* GetSceneName(Scene scenes) {
	switch (scenes) {
	case Scene::kTitle:
		return "Title";
	case Scene::kGame:
		return "Game";
	case Scene::kGameOver:
		return "Game Over";
	// case Scene::kGameClear:
	//     return "Game Clear";
	default:
		return "Unknown";
	}
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// タイトルシーン終了でゲームシーンへ
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// ゲームシーン終了でゲームオーバーシーンへ
			scene = Scene::kGameOver;
			delete gameScene;
			gameScene = nullptr;
			gameOverScene = new GameOverScene;
			gameOverScene->Initialize();
		}
		break;
	case Scene::kGameOver:
		if (gameOverScene->IsFinished()) {
			// ゲームオーバーシーン終了でタイトルシーンへ戻る
			scene = Scene::kTitle;
			delete gameOverScene;
			gameOverScene = nullptr;
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

	// タイトルシーンだけ初期化しておいて、シーンはタイトルからスタート
	titleScene = new TitleScene;
	titleScene->Initialize();
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

		// シーン切り替え処理
		ChangeScene();
		UpdateScene();

		// ImGuiにシーン名表示
		ImGui::Begin("Scene");
		ImGui::Text("Scene: %s", GetSceneName(scene));
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
	if (titleScene)
		delete titleScene;
	if (gameScene)
		delete gameScene;
	if (gameOverScene)
		delete gameOverScene;
	// if (gameClearScene) delete gameClearScene;

	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}
