#pragma once

#include <DirectXMath.h>
#include <cmath>
#include "Graphics/Model.h"

//ヒット結果
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0, 0, 0 };//レイとポリゴンの交点
	DirectX::XMFLOAT3 normal = { 0, 0, 0 };//衝突したポリゴンの法線ベクトル
	float distance = 0.0f;//レイの視点から交点までの距離
	int materialIndex = -1;//衝突したポリゴンのマテリアル番号
	DirectX::XMFLOAT3 rotetion = { 0, 0, 0 };
	DirectX::XMFLOAT3	triangleVerts[3];
};

//ポリゴン計算用
struct Triangle
{
	DirectX::XMFLOAT3	position[3] = {};
	int					materialIndex = -1;
};

//コリジョン
class Collision
{
public:
	//球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	//円柱と円柱の交差判定
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	//球と円柱の交差判定
	static bool IntersectSpherVsCyling(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	//レイとポリゴンの交差判定
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result
	);

	//レイと三角形ポリゴンの交差判定
	static bool IntersectRayVsTriangle(
		const DirectX::XMFLOAT3& rayStart,
		const DirectX::XMFLOAT3& rayDirection,		// 要正規化
		float rayDist,
		const DirectX::XMFLOAT3 triangleVerts[3],
		HitResult& result);
};

class QuadtreeNode
{
public:
	static const uint32_t	MAX_DEPTH = 10u;	// 最大分割数

	// 登録するオブジェクトを管理するマネージャー
	std::vector<Triangle>	triangles = {};		// 三角形登録用

	QuadtreeNode() {}
	QuadtreeNode(DirectX::XMFLOAT3 _center, float _halfSize) { this->center = _center; this->halfSize = _halfSize; }
	~QuadtreeNode() {}

private:
	DirectX::XMFLOAT3		center = {};		// 四分割空間の中心
	float					halfSize = {};		// 四分割空間の半辺長

public:
	DirectX::XMFLOAT3 GetCenter() const { return center; }
	void SetCenter(DirectX::XMFLOAT3 _center) { this->center = _center; }
	float GetHalfSize() const { return halfSize; }
	void SetHalfSize(float _halfSize) { this->halfSize = _halfSize; }

	// 線形四分割の階層のスタートインデックスの取得
	static const uint32_t GetLevelStart(uint32_t level)
	{
		// 等比数列の和の公式を利用することで求まるが定数なのであらかじめ計算しておく
		static const uint32_t levelStart[MAX_DEPTH] =
		{
			0, 1, 5, 21, 85, 341, 1365, 5461, 21845, 87381
		};
		return levelStart[level];
	}
	// 四分割空間の生成
	static void CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth, std::vector<QuadtreeNode>* node = {});

	// モートンコードの生成
	static uint32_t GetMortonCode(const DirectX::XMFLOAT3& point, const QuadtreeNode& route, float halfSize);
	static uint32_t bitSeparete(uint32_t n);

	// 四分割空間へ三角形オブジェクトを登録
	static void InsertTriangleObject(std::vector<QuadtreeNode>& node, Triangle& triangle, uint32_t depthMax);

	// 全ての三角形オブジェクトを四分割空間から削除
	static void ClearAllTriangleObject(std::vector<QuadtreeNode>& node);

	// 全てのノードで三角形と垂直レイの交差判定を行う
	static bool IntersectVerticalRayVsTriangle(std::vector<QuadtreeNode>* node, uint32_t depth, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResult& result);
};