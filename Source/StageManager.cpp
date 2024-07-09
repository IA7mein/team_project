#include "StageManager.h"

//�X�V����
void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
			stage->Update(elapsedTime);
	}
}

//�`�揈��
void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	for (Stage* stage : stages)
	{
		stage->Render(dc, shader);
	}
}

//�X�e�[�W�o�^
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

//�X�e�[�W�S�폜
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
//	//�S�ẴX�e�[�W�I�u�W�F�N�g�ɑ΂��ă��C�L���X�g���s���A
//	//�Փ˂�����_����ԋ߂������擾����
//	for (Stage* stage : stages)
//	{
//		if (stage->RayCast(start, end, hit))
//		{
//			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);//���C�̎n�_
//			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);//���C�̏I�_
//
//			// �ǂ̌�_
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