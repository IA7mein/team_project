#pragma once

#include "Player.h"
#include "Player2.h"
#include "Enemy.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

class Stage1 : public Scene
{
public:
	Stage1() {}
	~Stage1()override {}

	// ?ÅÒ??ÅÒ?
	void Initialize() override;

	// ?I??ÅÒ?
	void Finalize() override;

	// ?X?V????
	void Update(float elapsedTime) override;

	// ?`ÅÒ?????
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