#include "Collision.h"

//���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB
) {
	//A->B�ւ̒P�ʃx�N�g�����Z�o
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//��������
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}
	//�^�[�Q�b�g�����ւ̃x�N�g���𐳋K��
	Vec = DirectX::XMVector3Normalize(Vec);
	//���K�������x�N�g����range�X�P�[��
	Vec = DirectX::XMVectorScale(Vec, range);
	//�X�P�[�������x�N�g����PositionA�̈ʒu���瑫��
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	//�������l��XMFLOAT3�ɕϊ�
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}

//�~���Ɖ~���̌�������
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
	//A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}
	//A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}
	//XZ���ʂł͈̔̓`�F�b�N
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}
	//A��B�������o��
	vx /= distXZ;
	vz /= distXZ;
	outPositionB.x = positionA.x + (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (vz * range);
	return true;
}

bool Collision::IntersectSpherVsCyling(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
	//���̑������~���̓�����Ȃ瓖�����Ă��Ȃ�
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}
	//���̓����~���̑�����艺�Ȃ瓖�����Ă��Ȃ�
	if (spherePosition.y + sphereRadius < cylinderPosition.y)
	{
		return false;
	}

	//XZ���ʂł͈̔̓`�F�b�N
	//X���m������
	float vx = cylinderPosition.x - spherePosition.x;
	//Z���m������
	float vz = cylinderPosition.z - spherePosition.z;
	//XZ�̒������v�Z
	float distXZ = sqrtf(vx * vx + vz * vz);
	//���̔��a�Ɖ~���̒������v�Z����
	float range = sphereRadius + cylinderRadius;
	//XZ�̒��������̔��a�Ɖ~���̔��a�̒������傫����Γ������Ă��Ȃ�
	if (distXZ > range)
	{
		return false;
	}
	//�����~���������o��
	//vx��vz�𐳋K��
	vx /= distXZ;
	vz /= distXZ;

	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

	return true;
}

//���C�ƃ��f���̓����蔻��
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

	// ���[���h��Ԃ̃��C�̒������擾
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);

	bool hit = false;
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		//���b�V���m�[�h���擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		//���C�����[���h���W���烍�[�J�����W�ɕϊ�
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

		DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
		DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
		DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE); // Using SE for length calculation

		//���C�̒���
		float neart;
		DirectX::XMStoreFloat(&neart, Length); // Use SE length here
		//�ʂƂ̌�������
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
				//�O�p�`�̒��_�𒊏o
				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

				//�O�Ӄx�N�g���Z�o
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
				//�@���x�N�g���Z�o
				DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);
				//�\������
				DirectX::XMVECTOR D = DirectX::XMVector3Dot(V, N);
				float d;
				DirectX::XMStoreFloat(&d, D);

				if (d >= 0.0f)
					continue;

				//��_�Z�o
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S);
				DirectX::XMVECTOR X = DirectX::XMVectorDivide
				(DirectX::XMVector3Dot(SA, N), D);

				float x;
				DirectX::XMStoreFloat(&x, X);

				if (x < 0.0f || x > neart) continue;

				DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorMultiply(V, X));

				//��_�̕\������
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
				//��_�Ɩ@�����X�V
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
	const DirectX::XMFLOAT3& rayDirection,		// �v���K��
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

	if (d > 0.0f)	// �\������������Ă���Ƃ��̂ݔ�����s��
	{
		if (fabs(d) > 1e-6f)	// ���s�m�F
		{
			DirectX::XMVECTOR ap = DirectX::XMVectorSubtract(rayStartVec, trianglePos[0]);

			float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, ap));
			if (t >= 0.0f && t < d)		// ���C�̌����ƒ����m�F
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
// �l������Ԃ̐���
// �C���f�b�N�X�̑��������͂������̏���
void QuadtreeNode::CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth, std::vector<QuadtreeNode>* node)
{
	if (depth > MAX_DEPTH)
	{
		depth = MAX_DEPTH;
	}

	// ���[�g��Ԃ�o�^
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

// ���[�g���R�[�h�̐���
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

// �l������Ԃ֎O�p�`�I�u�W�F�N�g��o�^
void QuadtreeNode::InsertTriangleObject(std::vector<QuadtreeNode>& node, Triangle& triangle, uint32_t depthMax)
{
	// �O�p�`���܂�AABB���\������ŏ��_�ƍő�_���쐬�iy���W�������j
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

	// �O�p�`���܂�AABB�̍ŏ��_�̃��[�g���R�[�h
	int mortonCodeMin = GetMortonCode(minPoint, node.front(), node.back().halfSize);
	// �O�p�`���܂�AABB�̍ő�_�̃��[�g���R�[�h
	int mortonCodeMax = GetMortonCode(maxPoint, node.front(), node.back().halfSize);
	// ��L��̃��[�g���R�[�h�̔r���I�_���a
	int mortonCodeXOR = mortonCodeMin ^ mortonCodeMax;

	// ���[�g���R�[�h�̔r���I�_���a���g���A��ʋ�Ԃɓo�^���邩�̃`�F�b�N
	int i = 0;
	int shift = 0;
	while (mortonCodeXOR != 0)
	{
		// ����2bit���}�X�N���Ēl���`�F�b�N����
		if ((mortonCodeXOR & 0x3) != 0)
		{
			// �V�t�g����ۑ�
			shift = i + 1;
		}
		// 2bit�V�t�g�����čă`�F�b�N
		mortonCodeXOR >>= 2;
		i++;
	}

	// ���`�i�z��j�̃C���f�b�N�X�֕ϊ�
	int linerIndex = (mortonCodeMin >> (shift * 2)) + GetLevelStart(depthMax - shift);	// ���䐔��̘a�̌����𗘗p

	// �o�^
	if (linerIndex < node.size()) node[linerIndex].triangles.push_back(triangle);
}

// �S�Ă̎O�p�`�I�u�W�F�N�g���l������Ԃ���폜
void QuadtreeNode::ClearAllTriangleObject(std::vector<QuadtreeNode>& node)
{
	for (auto i = node.begin(); i != node.end(); i++)
	{
		i->triangles.clear();
	}
}

// �S�Ẵm�[�h�ŎO�p�`�Ɛ������C�̌���������s��
//bool QuadtreeNode::IntersectVerticalRayVsTriangle(std::vector<QuadtreeNode>* node, uint32_t depth, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResult& result)
//{
//	DirectX::XMFLOAT3 routeCenter = node->front().center;
//	float routeHalfSize = node->front().halfSize;
//	uint32_t shiftXZ = 0;
//	bool minus = false;
//	bool ret = false;
//
//	// ���C�̒����Ńq�b�g���̋�����������
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
//	// ���C�̎n�_�̃��[�g���R�[�h
//	uint32_t mortonCode = GetMortonCode(rayStart, node->front(), node->back().halfSize);
//	int linerIndex = mortonCode + GetLevelStart(depth);
//
//	for (uint32_t level = depth + 1; level > 0; level--)
//	{
//		if (linerIndex < node->size())
//		{
//			QuadtreeNode targetNode = node->at(linerIndex);
//
//			// �e�m�[�h�����O�p�`�S�Ăƃ��C�̓����蔻����s��
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
//		linerIndex = (linerIndex - 1) >> 2;	// �e�K�w�ɃC���f�b�N�X���ړ�
//	}
//
//	return ret;
//}