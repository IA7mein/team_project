#include "Collision.h"

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB
) {
	//A->Bへの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}
	//ターゲット方向へのベクトルを正規化
	Vec = DirectX::XMVector3Normalize(Vec);
	//正規化したベクトルをrangeスケール
	Vec = DirectX::XMVectorScale(Vec, range);
	//スケールしたベクトルをPositionAの位置から足す
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	//足した値をXMFLOAT3に変換
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositionB
)
{
	//Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}
	//Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}
	//XZ方面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}
	//AがBを押し出す
	vx /= distXZ;
	vz /= distXZ;
	outPositionB.x = positionA.x + (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (vz * range);
	return true;
}

bool Collision::IntersectSpherVsCyling(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
	//球の足元が円柱の頭より上なら当たっていない
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}
	//球の頭が円柱の足元より下なら当たっていない
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
	{
		return false;
	}

	//XZ方面での範囲チェック
	//X同士を引く
	float vx = cylinderPosition.x - spherePosition.x;
	//Z同士を引く
	float vz = cylinderPosition.z - spherePosition.z;
	//XZの長さを計算
	float distXZ = sqrtf(vx * vx + vz * vz);
	//球の半径と円柱の長さを計算する
	float range = sphereRadius + cylinderRadius;
	//XZの長瀬が球の半径と円柱の半径の長さより大きければ当たっていない
	if (distXZ > range)
	{
		return false;
	}
	//球が円柱を押し出す
	//vxとvzを正規化
	vx /= distXZ;
	vz /= distXZ;

	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

	return true;
}

//レイとモデルの当たり判定
bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const Model* model,
	HitResult& result
)
{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	// ワールド空間のレイの長さを取得
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		//メッシュノードを取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		//レイをワールド座標からローカル座標に変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE); // Using SE for length calculation

		//レイの長さ
		float neart;
		DirectX::XMStoreFloat(&neart, Length); // Use SE length here
		//面との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		int materialIndex = -1;
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));
				//三角形の頂点を抽出
				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				//三辺ベクトル算出
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
				//法線ベクトル算出
				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);
				//表裏判定
				DirectX::XMVECTOR D = DirectX::XMVector3Dot(V, N);
				float d;
				DirectX::XMStoreFloat(&d, D);

				if (d >= 0.0f)
					continue;

				//交点算出
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide
				(DirectX::XMVector3Dot(SA, N), D);

				float x;
				DirectX::XMStoreFloat(&x, X);

				if (x < 0.0f || x > neart) continue;

				DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorMultiply(V, X));

				//交点の表裏判定
				DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
				DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(N, Cross1);

				float d1;
				DirectX::XMStoreFloat(&d1, Dot1);

				if (d1 < 0.0f) continue;

				DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
				DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(N, Cross2);

				float d2;
				DirectX::XMStoreFloat(&d2, Dot2);

				if (d2 < 0.0f) continue;

				DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
				DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(N, Cross3);

				float d3;
				DirectX::XMStoreFloat(&d3, Dot3);

				if (d3 < 0.0f) continue;
				//交点と法線を更新
				neart = x;

				HitPosition = P;
				HitNormal = N;
				materialIndex = subset.materialIndex;
			}
		}
		if (materialIndex >= 0)
		{
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);

			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);

			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
};

bool Collision::IntersectRayVsTriangle(
	const DirectX::XMFLOAT3& rayStart,
	const DirectX::XMFLOAT3& rayDirection,		// 要正規化
	float rayDist,
	const DirectX::XMFLOAT3 triangleVerts[3],
	HitResult& result)
{
	DirectX::XMVECTOR rayStartVec = DirectX::XMLoadFloat3(&rayStart);
	DirectX::XMVECTOR rayDirectionVec = DirectX::XMLoadFloat3(&rayDirection);
	DirectX::XMVECTOR trianglePos[3];
	for (int i = 0; i < 3; i++)
	{
		trianglePos[i] = DirectX::XMLoadFloat3(&triangleVerts[i]);
	}

	DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(trianglePos[1], trianglePos[0]);
	DirectX::XMVECTOR ac = DirectX::XMVectorSubtract(trianglePos[2], trianglePos[0]);
	DirectX::XMVECTOR norm = DirectX::XMVector3Cross(ab, ac);
	DirectX::XMVECTOR qp = DirectX::XMVectorSubtract(rayStartVec, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, rayDist)));
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, qp));

	if (d > 0.0f)	// 表側から交差しているときのみ判定を行う
	{
		if (fabs(d) > 1e-6f)	// 平行確認
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStartVec, trianglePos[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// レイの向きと長さ確認
			{
				DirectX::XMVECTOR e = DirectX::XMVector3Cross(qp, ap);
				float v = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ac, e));
				if (v >= 0.0f && v <= d)
				{
					float w = -1 * DirectX::XMVectorGetX(DirectX::XMVector3Dot(ab, e));
					if (w > 0.0f && v + w <= d)
					{
						result.distance = rayDist * t / d;
						DirectX::XMStoreFloat3(&result.position, DirectX::XMVectorAdd(rayStartVec, DirectX::XMVectorScale(rayDirectionVec, result.distance)));
						DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(norm));
						for (int i = 0; i < 3; i++)
						{
							result.triangleVerts[i] = triangleVerts[i];
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}
// 四分割空間の生成
// インデックスの増加方向はｘ→ｚの順番
void QuadtreeNode::CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth, std::vector<QuadtreeNode>* node)
{
	if (depth > MAX_DEPTH)
	{
		depth = MAX_DEPTH;
	}

	// ルート空間を登録
	QuadtreeNode tmp(center, halfSize);
	node->push_back(tmp);

	for (uint32_t level = 0; level < depth; level++)
	{
		int levelStart = GetLevelStart(level);
		int loopEnd = static_cast<int>(std::pow(4.0f, (level)));
		for (int i = 0; i < loopEnd; i++)
		{
			center = node->at(i + levelStart).center;
			for (int j = 0; j < 4; j++)
			{
				tmp.center.x = center.x + halfSize * (j & 1 ? 0.5f : -0.5f);
				tmp.center.y = 0.0f;
				tmp.center.z = center.z + halfSize * (j & 2 ? 0.5f : -0.5f);
				tmp.halfSize = halfSize * 0.5f;
				node->push_back(tmp);
			}
		}
		halfSize *= 0.5f;
	}
}

// モートンコードの生成
uint32_t QuadtreeNode::GetMortonCode(const DirectX::XMFLOAT3& point, const QuadtreeNode& route, float halfSize)
{
	uint32_t x = static_cast<uint32_t>((point.x - (route.center.x - route.halfSize)) / (halfSize * 2));
	uint32_t z = static_cast<uint32_t>((point.z - (route.center.z - route.halfSize)) / (halfSize * 2));

	return bitSeparete(x) + (bitSeparete(z) << 1);
}
uint32_t QuadtreeNode::bitSeparete(uint32_t n)
{
	n = (n ^ (n << 8)) & 0x00ff00ff;
	n = (n ^ (n << 4)) & 0x0f0f0f0f;
	n = (n ^ (n << 2)) & 0x33333333;
	n = (n ^ (n << 1)) & 0x55555555;
	return n;
}

// 四分割空間へ三角形オブジェクトを登録
void QuadtreeNode::InsertTriangleObject(std::vector<QuadtreeNode>& node, Triangle& triangle, uint32_t depthMax)
{
	// 三角形を含むAABBを構成する最小点と最大点を作成（y座標を除く）
	DirectX::XMFLOAT3 minPoint = triangle.position[0];
	DirectX::XMFLOAT3 maxPoint = triangle.position[0];
	for (int i = 1; i < 3; i++)
	{
		if (triangle.position[i].x < minPoint.x)
		{
			minPoint.x = triangle.position[i].x;
		}
		if (triangle.position[i].z < minPoint.z)
		{
			minPoint.z = triangle.position[i].z;
		}

		if (triangle.position[i].x > maxPoint.x)
		{
			maxPoint.x = triangle.position[i].x;
		}
		if (triangle.position[i].z > maxPoint.z)
		{
			maxPoint.z = triangle.position[i].z;
		}
	}

	// 三角形を含むAABBの最小点のモートンコード
	int mortonCodeMin = GetMortonCode(minPoint, node.front(), node.back().halfSize);
	// 三角形を含むAABBの最大点のモートンコード
	int mortonCodeMax = GetMortonCode(maxPoint, node.front(), node.back().halfSize);
	// 上記二つのモートンコードの排他的論理和
	int mortonCodeXOR = mortonCodeMin ^ mortonCodeMax;

	// モートンコードの排他的論理和を使い、上位空間に登録するかのチェック
	int i = 0;
	int shift = 0;
	while (mortonCodeXOR != 0)
	{
		// 下位2bitずつマスクして値をチェックする
		if ((mortonCodeXOR & 0x3) != 0)
		{
			// シフト数を保存
			shift = i + 1;
		}
		// 2bitシフトさせて再チェック
		mortonCodeXOR >>= 2;
		i++;
	}

	// 線形（配列）のインデックスへ変換
	int linerIndex = (mortonCodeMin >> (shift * 2)) + GetLevelStart(depthMax - shift);	// 等比数列の和の公式を利用

	// 登録
	if (linerIndex < node.size()) node[linerIndex].triangles.push_back(triangle);
}

// 全ての三角形オブジェクトを四分割空間から削除
void QuadtreeNode::ClearAllTriangleObject(std::vector<QuadtreeNode>& node)
{
	for (auto i = node.begin(); i != node.end(); i++)
	{
		i->triangles.clear();
	}
}

// 全てのノードで三角形と垂直レイの交差判定を行う
//bool QuadtreeNode::IntersectVerticalRayVsTriangle(std::vector<QuadtreeNode>* node, uint32_t depth, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResult& result)
//{
//	DirectX::XMFLOAT3 routeCenter = node->front().center;
//	float routeHalfSize = node->front().halfSize;
//	uint32_t shiftXZ = 0;
//	bool minus = false;
//	bool ret = false;
//
//	// レイの長さでヒット情報の距離を初期化
//	DirectX::XMFLOAT3 rayDirection =
//	{
//		rayEnd.x - rayStart.x,
//		rayEnd.y - rayStart.y,
//		rayEnd.z - rayStart.z,
//	};
//	float rayDist = sqrtf(rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z * rayDirection.z);
//	rayDirection.x /= rayDist;
//	rayDirection.y /= rayDist;
//	rayDirection.z /= rayDist;
//
//	result.distance = rayDist;
//
//	// レイの始点のモートンコード
//	uint32_t mortonCode = GetMortonCode(rayStart, node->front(), node->back().halfSize);
//	int linerIndex = mortonCode + GetLevelStart(depth);
//
//	for (uint32_t level = depth + 1; level > 0; level--)
//	{
//		if (linerIndex < node->size())
//		{
//			QuadtreeNode targetNode = node->at(linerIndex);
//
//			// 各ノードが持つ三角形全てとレイの当たり判定を行う
//			for (int i = 0; i < targetNode.triangles.size(); i++)
//			{
//				HitResult tmpResult;
//				if (Collision::IntersectRayVsTriangle(rayStart, rayDirection, rayDist, targetNode.triangles.at(i).position, tmpResult))
//				{
//					if (result.distance > tmpResult.distance)
//					{
//						result = tmpResult;
//						result.materialIndex = targetNode.triangles.at(i).materialIndex;
//						ret = true;
//					}
//				}
//			}
//		}
//
//		linerIndex = (linerIndex - 1) >> 2;	// 親階層にインデックスを移動
//	}
//
//	return ret;
//}