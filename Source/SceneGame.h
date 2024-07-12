#pragma once

#include "Player.h"
#include "Enemy.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame()override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	Player* player = nullptr;
	Enemy* enemy = nullptr;
	CameraController* cameraController = nullptr;

	DirectX::XMFLOAT3 target = {0, 0, 0};
};
