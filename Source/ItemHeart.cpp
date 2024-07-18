#include "ItemHeart.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

//コンストラクタ
ItemHeart::ItemHeart()
{
	model = new Model("Data/Model/Item/hart.mdl");

	//モデルのスケーリング
	scale.x = scale.y = scale.z = 1.0f;

	//幅、高さ設定
	radius = 5.0f;
	height = 1.5f;
}

//デストラクタ
ItemHeart::~ItemHeart()
{
	delete model;
}

//更新処理
void ItemHeart::Update(float elapsedTime)
{
	

	//速力更新処理
	UpdateVelocity(elapsedTime);

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	//モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	//モデル行列更新
	model->UpdateTransform(transform);

	position.y = 5.4f;
}

//描画処理
void ItemHeart::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//ダメージを受けた際に呼ばれる
void ItemHeart::OnDamaged()
{
	
}

//死亡したときに呼ばれる
void ItemHeart::OnDead()
{
	
}

//デバッグプリミティブ描画
void ItemHeart::DrawDebugPrimitive()
{
	//基底クラスのデバッグプリミティブ描画
	Item::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
}

//縄張り設定
void ItemHeart::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}