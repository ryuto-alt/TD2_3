#include "GameScene.h"
#include "TextureManager.h"
#include "mymath.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete player;
	delete player2;
	delete player3;
	delete player4;
	delete player5; // 新しいプレイヤーの削除
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

	player = new Player();
	player2 = new Player();
	player3 = new Player();
	player4 = new Player();
	player5 = new Player(); // 新しいプレイヤーの初期化
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(8, 12);
	Vector3 playerPosition2 = mapChipField_->GetMapChipPositionByIndex(8, -1);
	Vector3 playerPosition3 = mapChipField_->GetMapChipPositionByIndex(-1, 5);
	Vector3 playerPosition4 = mapChipField_->GetMapChipPositionByIndex(16, 5);
	Vector3 playerPosition5 = mapChipField_->GetMapChipPositionByIndex(14, 5); // 新しいプレイヤーの座標
	player->Initialize(modelPlayer_, &viewProjection_, playerPosition);
	player2->Initialize(modelPlayer_, &viewProjection_, playerPosition2);
	player3->Initialize(modelPlayer_, &viewProjection_, playerPosition3);
	player4->Initialize(modelPlayer_, &viewProjection_, playerPosition4);
	player5->Initialize(modelPlayer_, &viewProjection_, playerPosition5); // 新しいプレイヤーの初期化

	// Camera
	Camera_ = new Camera();
	Camera_->Initialize(railcameraPos, railcameraRad);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {

	if (!ShotPlayer) {
		player->Update();
		player2->Update();
		player3->Update2();
		player4->Update2();
	}

	// player5が他のプレイヤーの近くにいるときに発射される方向を変更する
	Vector3 playerPos = player->GetWorldPosition();
	Vector3 player2Pos = player2->GetWorldPosition();
	Vector3 player3Pos = player3->GetWorldPosition();
	Vector3 player4Pos = player4->GetWorldPosition();
	Vector3 player5Pos = player5->GetWorldPosition();

	float distanceToPlayer3 = player5Pos.Distance(player3Pos);
	float distanceToPlayer4 = player5Pos.Distance(player4Pos);

	const float triggerDistance = 4.1f; // 発射をトリガーする距離

	if (distanceToPlayer3 < triggerDistance || distanceToPlayer4 < triggerDistance) {
		ShotPlayer = false;
		player5->Update2(); // 新しいプレイヤーの更新
	} else {
		player5->Update3();
	}

	if ((distanceToPlayer3 < triggerDistance) && Input::GetInstance()->PushKey(DIK_SPACE)) {
		Vector3 velocity = player5->GetVelocity();
		ShotPlayer = true;
		velocity.x = MapChipField::kBlockWidth / 2;
		player5->SetVelocity(velocity);
		if (velocity.x == 0) {
			ShotPlayer = false;
		}

		// player5のy座標をplayer3に合わせる
		Vector3 alignedPos = player5->GetWorldPosition();
		alignedPos.y = player3Pos.y;
		player5->SetWorldPosition(alignedPos);
	}

	if ((distanceToPlayer4 < triggerDistance) && Input::GetInstance()->PushKey(DIK_SPACE)) {
		Vector3 velocity = player5->GetVelocity();
		ShotPlayer = true;
		velocity.x = -MapChipField::kBlockWidth / 2;
		player5->SetVelocity(velocity);

		if (velocity.x == 0) {
			ShotPlayer = false;
		}
		// player5のy座標をplayer4に合わせる
		Vector3 alignedPos = player5->GetWorldPosition();
		alignedPos.y = player4Pos.y;
		player5->SetWorldPosition(alignedPos);
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

	player->Draw();
	player2->Draw();
	player3->Draw();
	player4->Draw();
	player5->Draw(); // 新しいプレイヤーの描画

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
