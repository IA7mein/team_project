#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

//�^�C�g���V�[��
class StageSelect :public Scene
{
public:
	StageSelect() {}
	~StageSelect() override {}

	//������
	void Initialize() override;

	//�I����
	void Finalize() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render() override;

private:
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;
	Sprite* sprite3 = nullptr;
	int statemode;
	int tablestage;
	int timer;
	float carsol_timer = 0.0f;
	float scene_timer = 0.0f;

	bool scene_change = false;

	std::unique_ptr<AudioSource>bgm;
	std::unique_ptr<AudioSource>PushButtan;
	std::unique_ptr<AudioSource>MoveCarsol;
};