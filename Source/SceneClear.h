#pragma once
#include "Graphics/Sprite.h"
#include "Scene.h"
#include "Player.h"
#include "Player2.h"
#include "enemy.h"

//タイトルシーン
class SceneClear :public Scene
{
public:
	SceneClear() {}
	~SceneClear() override {}

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
	Sprite* text = nullptr;
	int timer;
	bool scene_change = false;
	float scene_timer = 0.0f;

	std::unique_ptr<AudioSource>bgm;
	std::unique_ptr<AudioSource>PushButtan;
};