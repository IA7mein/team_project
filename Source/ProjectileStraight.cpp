#include "ProjectileStraight.h"
#include "Collision.h"
#include "StageManager.h"

ProjectileStraight::ProjectileStraight(ProjectileManager* manager)
	:Projectile(manager)
{
	model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");

	scale.x = scale.y = scale.z = 0.5f;
}

ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

void ProjectileStraight::Update(float elapsedTime)
{
	//��������
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		//�������폜
		Destroy();
	}
	//����
	float speed = this->speed * elapsedTime;
	//�ʒu += �O���� * ����
#if FALSE
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;
#endif

	float mx = direction.x * speed;
	float my = direction.y * speed;
	float mz = direction.z * speed;

	//���˂̏���
	{
		//���C�̊J�n�n�_�ƏI�_
		DirectX::XMFLOAT3 start = {position.x, position.y + 1.0f, position.z};
		DirectX::XMFLOAT3 end = {position.x + mx, position.y + 1.0f, position.z + mz};

		//���C�L���X�g�ɂ��ǂ̔���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			//�ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);//�ǂ̏Փˈʒu
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);//���C�̏I�_�ʒu
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start, End);//�ǂ̏Փˈʒu�܂ł̃x�N�g��

			//�ǂ̖@���x�N�g��
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//�ǂ̏Փˈʒu�܂ł̃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);
			float dot = DirectX::XMVectorGetX(Dot);//�ˉe�̒P��̒l

			//�ǂ̖@���x�N�g����dot��2�{���X�P�[�����O���iXMVECTORScale�j
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorScale(Normal, dot * 2.0f);
			//End�̈ʒu����CollectPosition���̃x�N�g���𑫂�
			DirectX::XMVECTOR ReflectPosition = DirectX::XMVectorAdd(End, CollectPosition);
			//ReflectPosition����hit.position�Ŕ��˃x�N�g�����v�Z
			DirectX::XMVECTOR ReflectVec = DirectX::XMVectorSubtract(ReflectPosition, DirectX::XMLoadFloat3(&hit.position));
			//���˃x�N�g���𐳋K��
			ReflectVec = DirectX::XMVector3Normalize(ReflectVec);

			//float�ɕϊ�
			DirectX::XMFLOAT3 reflectVec;
			DirectX::XMStoreFloat3(&reflectVec, ReflectVec);

			//�ʒu��x��z�̐�����hit.position�ɏ㏑��
			position.x = hit.position.x;			
			position.z = hit.position.z;
			
			//directio��x��z�̐������㏑��
			//(y�����ɂ��K�����邱�Ƃŏ�����ɔ��˂����邱�Ƃ��ł���)
			direction.x = reflectVec.x;
			direction.z = reflectVec.z;
		}
	}

	position.x += mx;
	position.y += my;
	position.z += mz;

	UpdateTransform();

	model->UpdateTransform(transform);
}

void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction,
	const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}