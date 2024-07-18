#include "Character.h"
#include "StageManager.h"
#include "Mathf.h"

//�s��X�V����

void Character::UpdateTransform()
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;

	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3�̍s���g�ݍ��킹�ă��[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	//�v�Z���ă��[���h���W�����o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

//�ړ�����
void Character::Move(float vx, float vz, float speed)
{
	//�ړ������x�N�g����ݒ�(�ړ������̖���ۑ����Ă���)
	moveVecX = vx;
	moveVecZ = vz;

	//�ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

//���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	//�i�s�x�N�g�����[���x�N�g���̏ꍇ�������Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f)return;

	//�i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	//���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	//���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	//�������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0f - dot;
	if (rot > speed) rot = speed;

	//���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontZ * vx) - (frontX * vz);

	//2D�̊O�ϒl�����̏ꍇ�����̏ꍇ���ɂ���č��E���肪�s����
	//���E������s�����Ƃɂ���č��E��]��I������
	if (cross < 0.0f)
	{
		//���̏ꍇ�͍���]
		angle.y -= rot;
	}
	else
	{
		//���̏ꍇ�͉E��]
		angle.y += rot;
	}
}
//�W�����v����
void Character::Jump(float speed)
{
	//������̗͂�ݒ�
	position.y = speed;
}

//�Ռ���^����
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	//���͂ɗ͂�������
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

//���͍X�V����
void Character::UpdateVelocity(float elapsedTime)
{
	//�o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	//�������͍X�V����(�o�߃t���[�����ƂɍX�V)
	UpdateVerticalVelocity(elapsedFrame);

	//�������͍X�V����
	UpdateHorizontalVelocity(elapsedFrame);

	//�����ړ��X�V����(�o�ߎ��Ԃ��ƂɍX�V)
	UpdateVerticalMove(elapsedTime);

	//�����ړ��X�V����
	UpdateHorizontalMove(elapsedTime);

}

bool Character::ApplyDamage(int damage, float invincibleTime)
{
	//�_���[�W��0�̏ꍇ�͌��N��Ԃ��X�V���Ȃ�
	if (damage == 0) return false;

	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0) return false;

	//���G���Ԓ��̓_���[�W���󂯂Ȃ�
	if (this->invincibleTimer > 0.0f) return false;

	//���G���Ԑݒ�
	this->invincibleTimer = invincibleTime;

	//�_���[�W����
	health -= damage;

	if (health <= 0)
	{
		OnDead();
	}
	else
	{
		OnDamaged();
	}

	return true;
}

//���G���ԍX�V
void Character::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

void Character::UpdateVerticalVelocity(float elapsedFrame)
{
	//�d�͏���
	velocity.y += gravity * elapsedFrame;
}

void Character::UpdateVerticalMove(float elapsedTime)
{
	/*if (position.y < 5.4f)
	{
		position.y = 5.4f;
	}*/
	//���������̈ړ���
	float my = velocity.y * elapsedTime;

	//�L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	slopeRate = 0.0f;

	//������
	if (my < 0.0f)
	position.y += velocity.y * elapsedTime;
	if (position.y <= 3.0f)
	{
		position.y = 3.0f;

			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
	}
		else
		{
			isGround = false;
		}
	////���������̈ړ���
	//float my = velocity.y * elapsedTime;

	////�L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	//DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	//slopeRate = 0.0f;

	////������
	//if (my < 0.0f)
	//{
	//	//���C�̊J�n�n�_�͑�����菭����
	//	DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
	//	//���C�̏I�_�ʒu�͈ړ���̈ʒu
	//	DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

	//	//���C�L���X�g�ɂ��n�ʔ���
	//	HitResult hit;
	//	if (StageManager::Instance().RayCast(start, end, hit))
	//	{
	//		//�@���x�N�g���擾
	//		normal = hit.normal;

	//		//�n�ʂɐڂ��Ă���
	//		position = hit.position;

	//		//��]
	//		angle = {angle.x + hit.rotetion.x, angle.y + hit.rotetion.y , angle.z + hit.rotetion.z };

	//		//�X�Η��̌v�Z
	//		float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
	//		slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

	////�n�ʂ̌����ɉ����悤��XZ����]
	//{
	//	//Y�����@���x�N�g�����ʂɌ����I�C���[�p��]���Z�o
	//	float angleX = static_cast<float>(atan2(normal.z, normal.y));
	//	float angleZ = static_cast<float>(-atan2(normal.x, normal.y));

	//	//���`�⊮�Ŋ��炩�ɉ�]
	//	angle.x = Mathf::Lerp(angle.x, angleX, 0.1f);
	//	angle.z = Mathf::Lerp(angle.z, angleZ, 0.1f);

	//}
}

//�������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
	//XZ���ʂ̑��͂���������
	float length = sqrtf((velocity.x * velocity.x + velocity.z * velocity.z));
	if (length > 0.0f)
	{
		//���C�W��
		float friction = this->friction * elapsedFrame;

		//�󒆂ɂ���Ƃ��͖��C�͂����炷
		if (this->isGround == false) friction *= airControl;

		//���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			//���͂�P�ʃx�N�g����(XMVector2Nolmalize)
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			//�P�ʃx�N�g�����������͂𖀎C�͕��X�P�[�����O�����l�𑬗͂������
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}

		//�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//XZ���ʂ̑��͂���������
	if (length <= maxMoveSpeed)
	{
		//�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			//������
			float accelaration = this->acceleration * elapsedFrame;

			//�󒆂ɂ���Ƃ��͉����͂����炷
			if (this->isGround == false) accelaration *= airControl;

			//�ړ��x�N�g���ɂ���������(�ړ��x�N�g���ɉ����͕��X�P�[�����O���A���x�x�N�g���ɒǉ�)
			velocity.x += moveVecX * accelaration;
			velocity.z += moveVecZ * accelaration;

			//�ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				//���x�x�N�g���𐳋K��
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				//�ő呬�����X�P�[�����O�����l�𑬓x�x�N�g���ɑ��
				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;
			}
			//�����ŃK�^�K�^���Ȃ��悤�ɂ���
			if (isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * elapsedFrame;
			}
		}
	}
	//�ړ��x�N�g�������Z�b�g
	moveVecX = 0.0f;
	moveVecZ = 0.0f;


}


void Character::UpdateHorizontalMove(float elapsedTime)
{
	// �������͌v�Z
	float velocityLengthXZ = static_cast<float>(sqrt(velocity.x * velocity.x + velocity.z * velocity.z));
	if (velocityLengthXZ > 0.0f)
	{
		// �����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		//�v���C���[��Y���𒆐S�ɐݒ肵�A���Ɣ��肵�Ȃ��悤�ɂ���
		//DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		//DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

		//// ���C�L���X�g�ɂ��ǔ���
		//HitResult hit;
		//if (StageManager::Instance().RayCast(start, end, hit))
		//{
		//	// �ǂ܂ł̃x�N�g��
		//	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
		//	DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
		//	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start, End);

		//	// �ǂ̖@���x�N�g��
		//	DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);
		//	// �i�s�����x�N�g���ƕǂ̖@���x�N�g���̓���
		//	DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);
		//	//�␳�ʒu�̌v�Z
		//	//�@���x�N�g��������Dot���X�P�[�����O
		//	DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiply(Normal, Dot);
		//	DirectX::XMFLOAT3 collectPosition;
		//	//CollectPosition��End�̈ʒu�𑫂����l���ŏI�I�Ȉʒu
		//	DirectX::XMStoreFloat3(&collectPosition, DirectX::XMVectorAdd(CollectPosition, End));

		//	//hit.position���J�n�ʒu�Ƃ��Acollectposition���I�_�ʒu�Ƃ��A����Ƀ��C�L���X�g
		//	HitResult hit2;
		//	if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2))
		//	{
		//		//�������ĂȂ��Ȃ�
		//		//X,Z�����̂ݔ��f
		//		position.x = collectPosition.x;
		//		position.z = collectPosition.z;
		//	}
		//	else
		//	{
		//		//�������Ă�����hit2.position���ŏI�I�Ȉʒu�Ƃ��Ĕ��f
		//		position.x = hit2.position.x;
		//		position.z = hit2.position.z;
		//	}
		//	
		//}
		/*else*/
		if (position.x > 24.6f && velocity.x > 0)
			{
			position.z += mz;
			}
			else if(position.x > 24.6f && velocity.x > 0)
		{
			position.z += mz;
		}
		else
		{
			// �ړ�
			position.x += mx;
			position.z += mz;
		}
	}
}