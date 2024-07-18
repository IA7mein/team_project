#pragma once

#include "Player.h"
#include "Player2.h"
#include "Enemy.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

class Stage2 : public Scene
{
public:
	Stage2() {}
	~Stage2()override {}

	// ‰Šú‰»
	void Initialize() override;

	// I—¹‰»
	void Finalize() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render() override;


private:
	Player* player = nullptr;
	Player2* player2 = nullptr;
	Enemy* enemy = nullptr;
	Sprite* text = nullptr;
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	std::unique_ptr<AudioSource>bgm;
};