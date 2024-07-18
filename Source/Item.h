#pragma once

#include "Graphics/Graphics.h"
#include "Character.h"

//�G�l�~�[
class Item : public Character
{
public:
	Item() {}
	~Item() override {}

	//�X�V����
	virtual void Update(float elapsedTime) = 0;

	//�`�揈��
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//�f�o�b�O�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	//�j��
	void Destoroy();

};