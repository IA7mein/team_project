#pragma once

#include <Vector>
#include "Item.h"
#include <set>

//�A�C�e���}�l�[�W���[
class ItemManager
{
private:
	ItemManager() {}
	~ItemManager() {}

public:
	//�B��̃C���X�^���X�擾
	static ItemManager& Instance()
	{
		static ItemManager instance;
		return instance;
	}

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//�A�C�e���o�^
	void Register(Item* Item);

	//�A�C�e���S�폜
	void Clear();

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//�A�C�e�����擾
	int GetItemCount() const { return static_cast<int>(enemies.size()); }

	//�A�C�e���擾
	Item* GetItem(int index) { return enemies.at(index); }

	//�A�C�e���폜
	void Remove(Item* Item);

private:

private:
	std::vector<Item*> enemies;
	std::set<Item*> removes;
};