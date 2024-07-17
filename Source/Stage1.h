#pragma once
#pragma once
#include "Player.h"
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
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;
};