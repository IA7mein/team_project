#include <vector>
#include "ItemManager.h"
#include "Collision.h"

void ItemManager::Update(float elapsedTime)
{
	for (Item* item : enemies)
	{
		item->Update(elapsedTime);
	}

	//�j������	
	for (Item* item : removes)
	{
		std::vector<Item*>::iterator it = std::find(enemies.begin(), enemies.end(), item);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		//�G�̔j��
		delete item;
	}
	////�p�����X�g���N���A
	removes.clear();

}

//�G�l�~�[�폜
void ItemManager::Remove(Item* Item)
{
	//�j�����X�g�ɒǉ�
	removes.insert(Item);
}

void ItemManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Item* Item : enemies)
	{
		Item->Render(context, shader);
	}
}

void ItemManager::DrawDebugPrimitive()
{
	for (Item* Item : enemies)
	{
		Item->DrawDebugPrimitive();
	}
}

void ItemManager::Register(Item* Item)
{
	enemies.emplace_back(Item);
}

void ItemManager::Clear()
{
	for (Item* item : enemies)
	{
		delete item;
	}
	enemies.clear();
}
