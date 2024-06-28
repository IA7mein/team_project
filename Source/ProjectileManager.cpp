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
	//更新処理
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsedTime);
	}

	//破棄処理
	for (Projectile* projectile : removes)
	{
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);
		
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		//弾丸の破棄
		delete projectile;
	}
	//廃棄リストをクリア
	removes.clear();
}

//描画処理
void ProjectileManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(context, shader);
	}
}

//デバッグプリミティブ描画
void ProjectileManager::DrawDebugPrimitive()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->DebugPrimitive();
	}
}

//弾丸登録
void ProjectileManager::Resister(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

//弾丸全削除
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

//弾丸削除
void ProjectileManager::Remove(Projectile* projectile)
{
	//破棄リストに追加
	removes.insert(projectile);
}