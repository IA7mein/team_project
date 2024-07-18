#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

//�^�C�g���V�[��
class SceneRule :public Scene
{
public:
	SceneRule() {}
	~SceneRule() override {}

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

	int timer;
	bool scene_change = false;
	float scene_timer = 0.0f;

	std::unique_ptr<AudioSource>bgm;
	std::unique_ptr<AudioSource>PushButtan;
};