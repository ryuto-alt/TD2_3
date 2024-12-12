#include "GameScene.h"
#include "TextureManager.h"
#include "mymath.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	BlockModel = Model::CreateFromOBJ("cube", true);

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	GenerateBlcoks();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_2)) {
		finished_ = true;
	}

#pragma region ブロック描画
	// 縦横ブロック更新
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			// アフィン変換行列の作成
			//(MakeAffineMatrix：自分で作った数学系関数)
			worldTransformBlockYoko->matWorld_ = MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

			// 定数バッファに転送
			worldTransformBlockYoko->TransferMatrix();
		}
	}
#pragma endregion

#pragma region デバッグカメラ
	// カメラ処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		/*viewProjection_.matView = cameraController_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;*/
		// ビュープロジェクションの転送
		viewProjection_.TransferMatrix();
	}

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == true)
			isDebugCameraActive_ = false;
		else
			isDebugCameraActive_ = true;
	}
#endif

#pragma endregion
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 縦横ブロック描画
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			BlockModel->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlcoks() {
	uint32_t numBlockVirticle = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirticle);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::lBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
