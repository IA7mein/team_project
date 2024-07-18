#pragma once

#include <Vector>
#include "Item.h"
#include <set>

//アイテムマネージャー
class ItemManager
{
private:
	ItemManager() {}
	~ItemManager() {}

public:
	//唯一のインスタンス取得
	static ItemManager& Instance()
	{
		static ItemManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//アイテム登録
	void Register(Item* Item);

	//アイテム全削除
	void Clear();

	//デバッグプリミティブ描画
	void DrawDebugPrimitive();

	//アイテム数取得
	int GetItemCount() const { return static_cast<int>(enemies.size()); }

	//アイテム取得
	Item* GetItem(int index) { return enemies.at(index); }

	//アイテム削除
	void Remove(Item* Item);

private:

private:
	std::vector<Item*> enemies;
	std::set<Item*> removes;
};