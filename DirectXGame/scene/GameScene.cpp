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

	// playerCenterが他のプレイヤーの近くにいるときに発射される方向を変更する
	Vector3 playerPos = playerBottom->GetWorldPosition();
	Vector3 playerTopPos = playerTop->GetWorldPosition();
	Vector3 playerLeftPos = playerLeft->GetWorldPosition();
	Vector3 playerRightPos = playerRight->GetWorldPosition();
	Vector3 playerCenterPos = playerCenter->GetWorldPosition();

	float distanceToplayerLeft = playerCenterPos.Distance(playerLeftPos);
	float distanceToplayerRight = playerCenterPos.Distance(playerRightPos);

	const float triggerDistance = 4.3f; // 発射をトリガーする距離

	if (distanceToplayerLeft < triggerDistance || distanceToplayerRight < triggerDistance) {
		ShotPlayer = false;
		playerCenter->Update2(); // 新しいプレイヤーの更新
	} else {
		playerCenter->Update3();
	}

	if ((distanceToplayerLeft < triggerDistance) && Input::GetInstance()->PushKey(DIK_SPACE)) {
		Vector3 velocity = playerCenter->GetVelocity();
		ShotPlayer = true;
		velocity.x = MapChipField::kBlockWidth / 2;
		playerCenter->SetVelocity(velocity);
		if (velocity.x == 0) {
			ShotPlayer = false;
		}

		// playerCenterのy座標をplayerLeftに合わせる
		Vector3 alignedPos = playerCenter->GetWorldPosition();
		alignedPos.y = playerLeftPos.y;
		playerCenter->SetWorldPosition(alignedPos);
	}

	if ((distanceToplayerRight < triggerDistance) && Input::GetInstance()->PushKey(DIK_SPACE)) {
		Vector3 velocity = playerCenter->GetVelocity();
		ShotPlayer = true;
		velocity.x = -MapChipField::kBlockWidth / 2;
		playerCenter->SetVelocity(velocity);

		if (velocity.x == 0) {
			ShotPlayer = false;
		}
		// playerCenterのy座標をplayerRightに合わせる
		Vector3 alignedPos = playerCenter->GetWorldPosition();
		alignedPos.y = playerRightPos.y;
		playerCenter->SetWorldPosition(alignedPos);
	}

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

	Camera_->Update();
	viewProjection_.matView = Camera_->GetViewProjection().matView;
	viewProjection_.matProjection = Camera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	ImGui::Begin("Scene");
	// ImGui::Text("playerPos3: %f",playerPosition3.y ); // シーン名を表示
	// ImGui::Text("playerPos4: %f",playerPosition4.y ); // シーン名を表示
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
