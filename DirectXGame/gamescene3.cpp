#include "GameScene3.h"
#include "TextureManager.h"
#include "mymath.h"
#include <cassert>

GameScene3::GameScene3() {}

GameScene3::~GameScene3() {
	delete player;
	delete player2;
	delete player3;
	delete player4;
	delete skydome_;
}

void GameScene3::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	NormalBlock = Model::CreateFromOBJ("cube", true);
	BomBlock = Model::CreateFromOBJ("bomblock", true);
	SlimeBlock = Model::CreateFromOBJ("slimeblock", true);
	EnemyBlock = Model::CreateFromOBJ("enemyblock", true);
	GoalBlock = Model::CreateFromOBJ("goalblock", true);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	GenerateBlcoks();

	modelPlayer_ = Model::CreateFromOBJ("cube", true);

	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	player = new Player();
	player2 = new Player();
	player->Initialize(modelPlayer_, &viewProjection_, playerPosition);
	player2->Initialize(modelPlayer_, &viewProjection_, playerPosition2);
	player3->Initialize(modelPlayer_, &viewProjection_, playerPosition3);
	player4->Initialize(modelPlayer_, &viewProjection_, playerPosition4);

	Camera_ = new Camera();
	Camera_->Initialize(railcameraPos, railcameraRad);

	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene3::Update() {
	player->Update();
	player2->Update();
	player3->Update2();
	player4->Update2();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		finished_ = true;
	}

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
	ImGui::Text("playerPos3: %f", playerPosition3.y);
	ImGui::Text("playerPos4: %f", playerPosition4.y);
	ImGui::End();
}

void GameScene3::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(commandList);

	player->Draw();
	player2->Draw();
	player3->Draw();
	player4->Draw();

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

	Model::PostDraw();

	Sprite::PreDraw(commandList);
	Sprite::PostDraw();
}

void GameScene3::GenerateBlcoks() {
	uint32_t numBlockVirticle = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

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
				worldTransform->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				worldTransformBlocks_[i][j] = worldTransform;
			}

			switch (chipType) {
			case MapChipType::lBlock:
				break;
			case MapChipType::bom:
				break;
			case MapChipType::enemy:
				break;
			case MapChipType::goal:
				break;
			case MapChipType::slime:
				break;
			default:
				break;
			}
		}
	}
}