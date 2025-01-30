#include "GameScene.h"
#include "TextureManager.h"
#include "mymath.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete playerBottom;
	delete playerTop;
	delete playerLeft;
	delete playerRight;
	delete playerCenter; // 新しいプレイヤーの削除
	delete skydome_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	NormalBlock = Model::CreateFromOBJ("cube", true);
	BomBlock = Model::CreateFromOBJ("bomblock", true);
	SlimeBlock = Model::CreateFromOBJ("slimeblock", true);
	EnemyBlock = Model::CreateFromOBJ("enemyblock", true);
	GoalBlock = Model::CreateFromOBJ("goalblock", true);

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	GenerateBlcoks();

	modelPlayer_ = Model::CreateFromOBJ("cube", true);

	// SkyDome
	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	playerBottom = new Player();
	playerTop = new Player();
	playerLeft = new Player();
	playerRight = new Player();
	playerCenter = new Player(); // 新しいプレイヤーの初期化
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(8, 12);
	Vector3 playerPosition2 = mapChipField_->GetMapChipPositionByIndex(8, -1);
	Vector3 playerPosition3 = mapChipField_->GetMapChipPositionByIndex(-1, 5);
	Vector3 playerPosition4 = mapChipField_->GetMapChipPositionByIndex(16, 5);
	Vector3 playerPosition5 = mapChipField_->GetMapChipPositionByIndex(14, 5); // 新しいプレイヤーの座標
	playerBottom->Initialize(modelPlayer_, &viewProjection_, playerPosition);
	playerTop->Initialize(modelPlayer_, &viewProjection_, playerPosition2);
	playerLeft->Initialize(modelPlayer_, &viewProjection_, playerPosition3);
	playerRight->Initialize(modelPlayer_, &viewProjection_, playerPosition4);
	playerCenter->Initialize(modelPlayer_, &viewProjection_, playerPosition5); // 新しいプレイヤーの初期化

	// Camera
	Camera_ = new Camera();
	Camera_->Initialize(railcameraPos, railcameraRad);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {

	if (!ShotPlayer) {
		playerBottom->Update();
		playerTop->Update();
		playerLeft->Update2();
		playerRight->Update2();
	}

	// 各プレイヤーのワールド座標を取得
	Vector3 playerBottomPos = playerBottom->GetWorldPosition();
	Vector3 playerTopPos = playerTop->GetWorldPosition();
	Vector3 playerLeftPos = playerLeft->GetWorldPosition();
	Vector3 playerRightPos = playerRight->GetWorldPosition();
	Vector3 playerCenterPos = playerCenter->GetWorldPosition();

	// Qキーで追従方向を切り替え (左右 ↔ 上下)
	static bool followHorizontal = true;
	if (input_->TriggerKey(DIK_Q)) {
		followHorizontal = !followHorizontal;
	}

	// 追従する対象を決定 (左右 or 上下)
	if (followHorizontal) {
		// 左右方向で「2ブロック挟む」位置に追従
		float distLeft = playerCenterPos.Distance(playerLeftPos);
		float distRight = playerCenterPos.Distance(playerRightPos);

		if (distLeft < distRight) {
			playerCenter->SetWorldPosition(Vector3(
			    playerLeftPos.x + 2 * MapChipField::kBlockWidth, // 左プレイヤーから2ブロック右
			    playerLeftPos.y, playerLeftPos.z));
		} else {
			playerCenter->SetWorldPosition(Vector3(
			    playerRightPos.x - 2 * MapChipField::kBlockWidth, // 右プレイヤーから2ブロック左
			    playerRightPos.y, playerRightPos.z));
		}
	} else {
		// 上下方向で「2ブロック挟む」位置に追従
		float distTop = playerCenterPos.Distance(playerTopPos);
		float distBottom = playerCenterPos.Distance(playerBottomPos);

		if (distTop < distBottom) {
			playerCenter->SetWorldPosition(Vector3(
			    playerTopPos.x,
			    playerTopPos.y - 2 * MapChipField::kBlockHeight, // 上プレイヤーから2ブロック下
			    playerTopPos.z));
		} else {
			playerCenter->SetWorldPosition(Vector3(
			    playerBottomPos.x,
			    playerBottomPos.y + 2 * MapChipField::kBlockHeight, // 下プレイヤーから2ブロック上
			    playerBottomPos.z));
		}
	}

	// ゲーム終了フラグ (DIK_2)
	if (Input::GetInstance()->PushKey(DIK_2)) {
		finished_ = true;
	}

#pragma region ブロック描画
	// 縦横ブロック更新
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			worldTransformBlockYoko->matWorld_ = MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);
			worldTransformBlockYoko->TransferMatrix();
		}
	}

	Camera_->Update();
	viewProjection_.matView = Camera_->GetViewProjection().matView;
	viewProjection_.matProjection = Camera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	ImGui::Begin("Scene");
	ImGui::End();
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

	playerBottom->Draw();
	playerTop->Draw();
	playerLeft->Draw();
	playerRight->Draw();
	playerCenter->Draw(); // 新しいプレイヤーの描画

	for (uint32_t i = 0; i < worldTransformBlocks_.size(); ++i) {
		for (uint32_t j = 0; j < worldTransformBlocks_[i].size(); ++j) {
			if (!worldTransformBlocks_[i][j])
				continue;

			MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			if (chipType == MapChipType::lBlock) {
				NormalBlock->Draw(*worldTransformBlocks_[i][j], viewProjection_);
			} else if (chipType == MapChipType::bom) {
				BomBlock->Draw(*worldTransformBlocks_[i][j], viewProjection_);
			} else if (chipType == MapChipType::slime) {
				SlimeBlock->Draw(*worldTransformBlocks_[i][j], viewProjection_);
			} else if (chipType == MapChipType::enemy) {
				EnemyBlock->Draw(*worldTransformBlocks_[i][j], viewProjection_);
			} else if (chipType == MapChipType::goal) {
				GoalBlock->Draw(*worldTransformBlocks_[i][j], viewProjection_);
			}
		}
	}

	skydome_->Draw();

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

	// ブロック配列のサイズを調整
	worldTransformBlocks_.resize(numBlockVirticle);
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			MapChipType chipType = mapChipField_->GetMapChipTypeByIndex(j, i);

			if (chipType != MapChipType::kBlank) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				// 位置を設定
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

				worldTransformBlocks_[i][j] = worldTransform;
			}

			switch (chipType) {
			case MapChipType::lBlock:
				// ブロック1の処理
				break;
			case MapChipType::bom:
				// 爆弾の処理
				break;
			case MapChipType::enemy:
				// 敵の処理
				break;
			case MapChipType::goal:
				// ゴールの処理
				break;
			case MapChipType::slime:
				// スライムの処理
				break;
			default:
				break;
			}
		}
	}
}
