#pragma once
#include "Player.h"
#include "Enemy.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

class Stage3 : public Scene
{
public:
	Stage3() {}
	~Stage3()override {}

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
	Enemy* enemy = nullptr;
	Sprite* text = nullptr;
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	std::unique_ptr<AudioSource>bgm;
};