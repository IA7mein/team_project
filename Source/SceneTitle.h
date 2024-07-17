#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"


//�^�C�g���V�[��
class SceneTitle :public Scene
{
public:
	SceneTitle(){}
	~SceneTitle() override{}

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
	Sprite* spriterule = nullptr;
	Sprite* font;
	int statemode;
	int timer;
	int mode;
};
extern bool muluchmode;