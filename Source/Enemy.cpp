#include "Enemy.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"
#include "Graphics/Graphics.h"

static Enemy* instance = nullptr;

//�C���X�^���X�擾
Enemy& Enemy::Instance()
{
	return *instance;
}

//�R���X�g���N�^
Enemy::Enemy()
{
	//�C���X�^���X�|�C���^�ݒ�
	instance = this;

	model = new Model("Data/Model/Slime/Slime.mdl");

	JumpStart = Audio::Instance().LoadAudioSource("Data/SE/JumpStart.wav");
	JumpEnd = Audio::Instance().LoadAudioSource("Data/SE/JumpEnd.wav");
	Hit = Audio::Instance().LoadAudioSource("Data/SE/Hit.wav");

	//���f���̃X�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	//�p�j�X�e�[�g�֑J��
	TransitionWanderState();
}

//�f�X�g���N�^
Enemy::~Enemy()
{
	JumpStart->Stop();
	JumpEnd->Stop();
	Hit->Stop();

	delete model;
}

//�X�V����
void Enemy::Update(float elapsedTime)
{
	if (position.y < -10.0f)
	{
		position = { 0, 1, -10 };
		SetRandomTargetPosition();
	}
	//�X�e�[�g���̍X�V����
	switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	/*case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::IdleBattle:
		UpdateIdleBattleState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;*/
	}

	//���͍X�V����
	UpdateVelocity(elapsedTime);

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	CollisionPlayerVsEnemies();

	SetRandomTargetPosition();

	//���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	//���f���s��X�V
	model->UpdateTransform(transform);
}

//�`�揈��
void Enemy::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

////�_���[�W���󂯂��ۂɌĂ΂��
//void Enemy::OnDamaged()
//{
//	//�_���[�W�X�e�[�g�֑J��
//	TransitionDamageState();
//}
//
////���S�����Ƃ��ɌĂ΂��
//void Enemy::OnDead()
//{
//	//���S�X�e�[�g�֑J��
//	TransitionDeathState();
//}

//�f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	////�꒣��͈͂��f�o�b�O�~���`��
	debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
		DirectX::XMFLOAT4(0, 1, 0, 1));

	//�^�[�Q�b�g�ʒu���f�o�b�O���`��
	debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//���G�͈͂��f�o�b�O�~���`��
	debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	//�U���͈͂��f�o�b�O�~���`��
	debugRenderer->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void Enemy::eJump()
{	
		//�W�����v�񐔐���
	if (jumpCount < jumpLimit && jumpCT <= 0.0f)
	{
		JumpStart->Play(false);//�W�����vSE�Đ�
		jumpFlag = true;
		//�W�����v
		 jumpCount++;
		 Jump(jumpSpeed);
		 //�G�̈ʒu���v���C���[�Ɣ�ׂđ傫�����ǂ����ŗ͂̌�����ς���
		 DirectX::XMFLOAT3 Ppos = Player::Instance().GetPosition();
		 if (position.x - Ppos.x > 0.0f)
		 {
			 power = 1.0f;
		 }
		 else if (position.x - Ppos.x < 0.0f)
		 {
			 power = -1.0f;
		 }
	}
}

//���n�����ۂɌĂ΂��
void Enemy::OnLanding()
{
	if (jumpFlag == true)
	{
		JumpEnd->Play(false);//���nSE�Đ�
		jumpCT = 2.0f;
		jumpFlag = false;
		jumpCount = 0;
		if (moveSpeed >= 1.5f)
		{
			moveSpeed /= 2.0f;
		}
	}
}

//�v���C���[�ƃG�l�~�[�Ƃ̏Փ˔���
void Enemy::CollisionPlayerVsEnemies()
{
	Player& player = Player::Instance();
	//�Փˏ���
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition
	))
	{
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&player.GetPosition());
		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
		DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
		DirectX::XMFLOAT3 normal;
		DirectX::XMStoreFloat3(&normal, N);
		{
			float L{};
			DirectX::XMFLOAT3 l{};
			DirectX::XMStoreFloat3(&l, DirectX::XMVectorScale(V, -1.0f));
			if (l.x > 0.0f)
			{
				L = 1.0f;
			}
			else if (l.x < 0.0f)
			{
				L = -1.0f;
			}

			if (jumpFlag && !player.GetJump())
			{
				Hit->Play(false);//�U���̃q�b�g���Đ�
				outPosition.x = position.x + (2.0f * L);
				player.SetPosition(outPosition);
			}
			else if (power != 0.0f && player.GetPower() * -1.0f == power)
			{
				outPosition.x = position.x + ((player.GetRadius() * 0.5f) * L);
				player.SetPosition(outPosition);
			}
			else if (power != 0.0f && player.GetPower() == 0.0f)
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				player.SetPosition(outPosition);
			}
			else
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				player.SetPosition(outPosition);
			}
		}
	}
}

//�꒣��ݒ�
void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

//�^�[�Q�b�g�ʒu�������_���ɐݒ�
void Enemy::SetRandomTargetPosition()
{
	if ((position.x <= targetPosition.x + 0.5f && position.x >= targetPosition.x - 0.5f)
		|| (position.z <= targetPosition.z + 0.5f && position.z >= targetPosition.z - 0.5f))
	{
		targetRandomX = static_cast<int>(Mathf::RandomRange(-2.5f, 2.5f));
		
		if (targetRandomX >= 2 || targetRandomX <= -2)
		{
			eJump();
		}
		targetPosition.x = position.x + static_cast<float>(targetRandomX);
		targetPosition.y = 0.5f;
		targetPosition.z = position.z + 3.0f;
	}
}

//�ڕW�n�_�ֈړ�
void Enemy::MoveToTarget(float elapsedTime, float speedRate)
{
	moveSpeed += Velocity * elapsedTime;
	if (moveSpeed >= maxSpeed)
	{
		moveSpeed = maxSpeed;
	}
	if (jumpCT > 0.0f)
	{
		jumpCT -= 1.0f * elapsedTime;
	}
	//�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	if (vx != 0.0f && moveSpeed >= maxSpeed)
	{
		moveSpeed = 2.3f;
	}

	if (vx > 0.0f)
	{
		power = 1.0f;
	}
	else if (vx < 0.0f)
	{
		power = -1.0f;
	}
	else if (!jumpFlag && power != 0.0f)
	{
		//��W�����v���Ȃ�͂�0�ɖ߂�	
		power = 0.0f;
	}

	//�ړ�����
	if (jumpFlag)
	{
		Move(vx * 0.5f, vz, moveSpeed);
	}
	else Move(vx, vz, moveSpeed);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool Enemy::SearchPlayer()
{
	//�v���C���[�Ƃ̍��፷���l������3D�ł̋������������
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	targetRandomX = 5;
	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//�P�ʃx�N�g����
		vx /= distXZ;
		vz /= distXZ;
		//�O���x�N�g��
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		//2�̃x�N�g���̓��ϒl�őO�㔻��
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

//�m�[�h�ƃv���C���[�̏Փˏ���
void Enemy::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	//�m�[�h�擾
	Model::Node* node = model->FindNode(nodeName);
	//�m�[�h�ʒu�擾
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	Graphics::Instance().GetDebugRenderer()->DrawSphere(
		nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
	);

	Player& player = Player::Instance();

	//�Փˏ���
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectSpherVsCyling(
		nodePosition,
		nodeRadius,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		//�_���[�W��^����
		if (player.ApplyDamage(1, 0.5f))
		{
			//������΂�
			{
				DirectX::XMFLOAT3 impulse;
				const float power = 10.0f;//������΂���

				//�G�̈ʒu���擾
				const DirectX::XMFLOAT3& e = player.GetPosition();
				//�e�̈ʒu���擾
				const DirectX::XMFLOAT3& p = nodePosition;
				//�e����G�ւ̃x�N�g�����擾
				float vx = e.x - p.x;
				float vz = e.z - p.z;
				//���̃x�N�g���𐳋K��(�������v�Z���A�����Ŋ���)
				float lengthXZ = sqrtf(vx * vx + vz * vz);
				vx /= lengthXZ;
				vz /= lengthXZ;
				//�Ռ��̒l��ݒ�(xz�͐��K�������x�N�g����power���X�P�[�����O)
				impulse.x = vx * power;
				impulse.y = power * 0.5f;
				impulse.z = vz * power;

				player.AddImpulse(impulse);
			}
		}
	}
}


//�p�j�X�e�[�g�֑J��
void Enemy::TransitionWanderState()
{
	state = State::Wander;

	//�ڕW�n�_�ݒ�
	//SetRandomTargetPosition();
}

//�p�j�X�e�[�g�X�V����
void Enemy::UpdateWanderState(float elapsedTime)
{
	//�ڕW�n�_�܂�XZ���ʂł̋�������
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;
	if (distSq < radius * radius)
	{
		//�ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}
	//�ڕW�n�_�ֈړ�
	MoveToTarget(elapsedTime, 0.5f);
	SetTerritory(position, 10.0f);

	////�v���C���[���G
	//if (SearchPlayer())
	//{
	//	//����������ǐՃX�e�[�g�֑J��
	//	TransitionPursuitState();
	//}
}

//�ҋ@�X�e�[�g�֑J��
void Enemy::TransitionIdleState()
{
	state = State::Idle;

	//�^�C�}�[�������_���ݒ�
	stateTimer = static_cast<float>(Mathf::RandomRange(3, 5));
}

//�ҋ@�X�e�[�g�X�V����
void Enemy::UpdateIdleState(float elapsedTime)
{
	//�^�C�}�[����
	stateTimer -= elapsedTime;

	//�p�j�X�e�[�g�֑J��
	TransitionWanderState();
}

////�ǐՃX�e�[�g�֑J��
//void Enemy::TransitionPursuitState()
//{
//	state = State::Pursuit;
//
//	//���b�ԒǐՂ���^�C�}�[�������_���ݒ�
//	stateTimer = static_cast<float>(Mathf::RandomRange(3, 5));
//}
//
////�ǐՃX�e�[�g�X�V����
//void Enemy::UpdatePursuitState(float elapsedTime)
//{
//	//�ڕW�n�_�ֈړ�
//	MoveToTarget(elapsedTime, 1.0f);
//	SetTerritory(position, 10.0f);
//
//	//�^�C�}�[����
//	if (!SearchPlayer())
//	{
//		stateTimer -= elapsedTime;
//	}
//
//	if (stateTimer < 0.0f)
//	{
//		//�ҋ@�X�e�[�g�֑J��
//		TransitionIdleState();
//	}
//
//	float vx = targetPosition.x - position.x;
//	float vy = targetPosition.y - position.y;
//	float vz = targetPosition.z - position.z;
//	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//	if (dist < attackRange)
//	{
//		//�U���X�e�[�g�֑J��
//		TransitionAttackState();
//	}
//}
//
////�U���X�e�[�g�֑J��
//void Enemy::TransitionAttackState()
//{
//	state = State::Attack;
//}
//
////�U���X�e�[�g�X�V����
//void Enemy::UpdateAttackState(float elapsedTime)
//{
//	//�C�ӂ̃A�j���[�V������Ԃł̂ݏՓ˔��菈��������
//	float animationTime = model->GetCurrentAnimetionSeconds();
//	if (animationTime >= 0.1f && animationTime <= 0.35f)
//	{
//		//�ڋʂ̃m�[�h�ƃv���C���[�̏Փ˔���
//		CollisionNodeVsPlayer("EyeBall", 0.2f);
//	}
//	//�U���A�j���[�V�������I��������퓬�ҋ@
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleBattleState();
//	}
//}
//
////�퓬�ҋ@�X�e�[�g�֑J��
//void Enemy::TransitionIdleBattleState()
//{
//	state = State::IdleBattle;
//
//	//���b�ԑҋ@����^�C�}�[�������_���Őݒ�
//	stateTimer = 0.0f;
//}
//
////�퓬�ҋ@�X�e�[�g�X�V����
//void Enemy::UpdateIdleBattleState(float elapsedTime)
//{
//	DirectX::XMFLOAT3 PlayerPosition = Player::Instance().GetPosition();
//	//�^�C�}�[����
//	stateTimer -= elapsedTime;
//	if (stateTimer < 0.0f)
//	{
//		//�v���C���[���U���͈͂ɂ���΍U���X�e�[�g�֑J��
//		float vx = PlayerPosition.x - position.x;
//		float vy = PlayerPosition.y - position.y;
//		float vz = PlayerPosition.z - position.z;
//		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//		if (dist < attackRange)
//		{
//			//�U���X�e�[�g�֑J��
//			TransitionAttackState();
//		}
//		else
//		{
//			//�p�j�X�e�[�g�֑J��
//			TransitionWanderState();
//		}
//	}
//	MoveToTarget(elapsedTime, 0.0f);
//}
//
////�_���[�W�X�e�[�g�֑J��
//void Enemy::TransitionDamageState()
//{
//	state = State::Damage;
//}
//
////�_���[�W�X�e�[�g�X�V����
//void Enemy::UpdateDamageState(float elapsedTime)
//{
//	//�_���[�W�A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleBattleState();
//	}
//}
//
////���S�X�e�[�g�֑J��
//void Enemy::TransitionDeathState()
//{
//	state = State::Death;
//}
//
////���S�X�e�[�g�X�V����
//void Enemy::UpdateDeathState(float elapsedTime)
//{
//	//�A�j���[�V�������I������玩�M��j��
//	if (!model->IsPlayAnimation())
//	{
//		
//	}
//}