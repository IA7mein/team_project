#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

//タイトルシーン
class SceneTitle :public Scene
{
public:
	SceneTitle(){}
	~SceneTitle() override{}

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

	float scene_timer = 0.0f;
	bool scene_change = false;
	//オーディオリソース
	std::unique_ptr<AudioSource>bgm;
	std::unique_ptr<AudioSource>PushButtan;
	std::unique_ptr<AudioSource>MoveCarsol;
};