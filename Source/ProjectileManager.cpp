#include "ProjectileManager.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
	Clear();
}

void ProjectileManager::Update(float elapsedTime)
{
	//�X�V����
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}

	//�j������
	for (Projectile* projectile : removes)
	{
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		//�e�ۂ̔j��
		delete projectile;
	}
	//�p�����X�g���N���A
	removes.clear();
}

//�`�揈��
void ProjectileManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(context, shader);
	}
}

//�f�o�b�O�v���~�e�B�u�`��
void ProjectileManager::DrawDebugPrimitive()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->DebugPrimitive();
	}
}

//�e�ۓo�^
void ProjectileManager::Resister(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

//�e�ۑS�폜
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

//�e�ۍ폜
void ProjectileManager::Remove(Projectile* projectile)
{
	//�j�����X�g�ɒǉ�
	removes.insert(projectile);
}