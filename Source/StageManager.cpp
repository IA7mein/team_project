#include "StageManager.h"

//更新処理
void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
			stage->Update(elapsedTime);
	}
}

//描画処理
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Stage* stage : stages)
	{
		stage->Render(dc, shader);
	}
}

//ステージ登録
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

//ステージ全削除
void StageManager::Clear()
{
	for (Stage* stage : stages)
	{
		delete stage;
	}
	stages.clear();
}

//bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
//{
//	bool result = false;
//	float Max_distance = FLT_MAX;
//
//	//全てのステージオブジェクトに対してレイキャストを行い、
//	//衝突した交点が一番近い情報を取得する
//	for (Stage* stage : stages)
//	{
//		if (stage->RayCast(start, end, hit))
//		{
//			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);//レイの始点
//			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);//レイの終点
//
//			// 壁の交点
//			DirectX::XMVECTOR HitPosition = DirectX::XMLoadFloat3(&hit.position);
//			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(HitPosition, Start);
//			float distance = DirectX::XMVectorGetX(Vec);
//
//			if (distance < Max_distance)
//			{
//				Max_distance = distance;
//				result = true;
//			}
//		}
//	}
//	return result;
//}