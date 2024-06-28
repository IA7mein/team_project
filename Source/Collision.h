#pragma once

#include <DirectXMath.h>
#include <cmath>
#include "Graphics/Model.h"

//�q�b�g����
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0, 0, 0 };//���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal = { 0, 0, 0 };//�Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f;//���C�̎��_�����_�܂ł̋���
	int materialIndex = -1;//�Փ˂����|���S���̃}�e���A���ԍ�
	DirectX::XMFLOAT3 rotetion = { 0, 0, 0 };
	DirectX::XMFLOAT3	triangleVerts[3];
};

//�|���S���v�Z�p
struct Triangle
{
	DirectX::XMFLOAT3	position[3] = {};
	int					materialIndex = -1;
};

//�R���W����
class Collision
{
public:
	//���Ƌ��̌�������
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB
	);

	//�~���Ɖ~���̌�������
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	//���Ɖ~���̌�������
	static bool IntersectSpherVsCyling(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	//���C�ƃ|���S���̌�������
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result
	);

	//���C�ƎO�p�`�|���S���̌�������
	static bool IntersectRayVsTriangle(
		const DirectX::XMFLOAT3& rayStart,
		const DirectX::XMFLOAT3& rayDirection,		// �v���K��
		float rayDist,
		const DirectX::XMFLOAT3 triangleVerts[3],
		HitResult& result);
};

class QuadtreeNode
{
public:
	static const uint32_t	MAX_DEPTH = 10u;	// �ő啪����

	// �o�^����I�u�W�F�N�g���Ǘ�����}�l�[�W���[
	std::vector<Triangle>	triangles = {};		// �O�p�`�o�^�p

	QuadtreeNode() {}
	QuadtreeNode(DirectX::XMFLOAT3 _center, float _halfSize) { this->center = _center; this->halfSize = _halfSize; }
	~QuadtreeNode() {}

private:
	DirectX::XMFLOAT3		center = {};		// �l������Ԃ̒��S
	float					halfSize = {};		// �l������Ԃ̔��Ӓ�

public:
	DirectX::XMFLOAT3 GetCenter() const { return center; }
	void SetCenter(DirectX::XMFLOAT3 _center) { this->center = _center; }
	float GetHalfSize() const { return halfSize; }
	void SetHalfSize(float _halfSize) { this->halfSize = _halfSize; }

	// ���`�l�����̊K�w�̃X�^�[�g�C���f�b�N�X�̎擾
	static const uint32_t GetLevelStart(uint32_t level)
	{
		// ���䐔��̘a�̌����𗘗p���邱�Ƃŋ��܂邪�萔�Ȃ̂ł��炩���ߌv�Z���Ă���
		static const uint32_t levelStart[MAX_DEPTH] =
		{
			0, 1, 5, 21, 85, 341, 1365, 5461, 21845, 87381
		};
		return levelStart[level];
	}
	// �l������Ԃ̐���
	static void CreateQuadtree(DirectX::XMFLOAT3 center, float halfSize, uint32_t depth, std::vector<QuadtreeNode>* node = {});

	// ���[�g���R�[�h�̐���
	static uint32_t GetMortonCode(const DirectX::XMFLOAT3& point, const QuadtreeNode& route, float halfSize);
	static uint32_t bitSeparete(uint32_t n);

	// �l������Ԃ֎O�p�`�I�u�W�F�N�g��o�^
	static void InsertTriangleObject(std::vector<QuadtreeNode>& node, Triangle& triangle, uint32_t depthMax);

	// �S�Ă̎O�p�`�I�u�W�F�N�g���l������Ԃ���폜
	static void ClearAllTriangleObject(std::vector<QuadtreeNode>& node);

	// �S�Ẵm�[�h�ŎO�p�`�Ɛ������C�̌���������s��
	static bool IntersectVerticalRayVsTriangle(std::vector<QuadtreeNode>* node, uint32_t depth, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd, HitResult& result);
};