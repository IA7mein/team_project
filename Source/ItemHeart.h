#pragma once

#include "Graphics/Graphics.h"
#include "Item.h"

//スライム
class ItemHeart : public Item
{
public:
	ItemHeart();
	~ItemHeart()override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	//縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

protected:
	//死亡したときに呼ばれる
	void OnDead() override;

	//ダメージを受けた際に呼ばれる
	void OnDamaged() override;

private:


private:


	Model* model = nullptr;
	
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0, 0, 0 };
	float territoryRange = 10.0f;
	float moveSpeed = 3.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 5.0f;
	float attackRange = 1.5f;
};

