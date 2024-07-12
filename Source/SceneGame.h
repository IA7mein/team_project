#pragma once

#include "Player.h"
#include "Enemy.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame()override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	CameraController* cameraController = nullptr;

	DirectX::XMFLOAT3 target = {0, 0, 0};
};
