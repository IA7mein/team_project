#pragma once
#include "Graphics/Sprite.h"
#include "Scene.h"



class SceneClear :public Scene
{
public:
	SceneClear() {}
	~SceneClear() override {}

	//‰Šú‰»
	void Initialize() override;

	//I—¹‰»
	void Finalize() override;

	//XVˆ—
	void Update(float elapsedTime) override;

	//•`‰æˆ—
	void Render() override;

private:
	Sprite* sprite = nullptr;
	
};
