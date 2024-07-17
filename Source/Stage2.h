#pragma once
#pragma once
#include "Player.h"
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
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;
};