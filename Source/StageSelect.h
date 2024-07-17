#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

//タイトルシーン
class StageSelect :public Scene
{
public:
	StageSelect() {}
	~StageSelect() override {}

	//初期化
	void Initialize() override;

	//終了化
	void Finalize() override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render() override;

private:
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;
	Sprite* sprite3 = nullptr;
	int statemode;
	int tablestage;
	int timer;
};