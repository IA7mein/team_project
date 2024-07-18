#include "Item.h"
#include "Graphics/Graphics.h"
#include "ItemManager.h"

void Item::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//衝突判定用のデバッグ球描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//破棄
void Item::Destoroy()
{
	//敵マネージャーの削除に自分自身のアドレスを渡す
	ItemManager::Instance().Remove(this);
}