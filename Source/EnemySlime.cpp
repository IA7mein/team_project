#include "EnemySlime.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

//�R���X�g���N�^
EnemySlime::EnemySlime()
{
	model = new Model("Data/Model/Slime/Slime.mdl");

	//���f���̃X�P�[�����O
	scale.x = scale.y = scale.z = 0.05f;

	//���A�����ݒ�
	radius = 5.0f;
	height = 1.5f;

	//�p�j�X�e�[�g�֑J��
	TransitionWanderState();
}

//�f�X�g���N�^
EnemySlime::~EnemySlime()
{
	delete model;
}

//�X�V����
void EnemySlime::Update(float elapsedTime)
{
	//�X�e�[�g���̍X�V����
	/*switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	case State::Pursuit:
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
		break;
	}*/

	//���͍X�V����
	UpdateVelocity(elapsedTime);

	//���G���ԍX�V
	UpdateInvincibleTimer(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	//���f���s��X�V
	model->UpdateTransform(transform);

	position.y = 5.4f;
}

//�`�揈��
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//�_���[�W���󂯂��ۂɌĂ΂��
void EnemySlime::OnDamaged()
{
	//�_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

//���S�����Ƃ��ɌĂ΂��
void EnemySlime::OnDead()
{
	//���S�X�e�[�g�֑J��
	TransitionDeathState();
}

//�f�o�b�O�v���~�e�B�u�`��
void EnemySlime::DrawDebugPrimitive()
{
	//���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//////�꒣��͈͂��f�o�b�O�~���`��
	//debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
	//	DirectX::XMFLOAT4(0, 1, 0, 1));

	////�^�[�Q�b�g�ʒu���f�o�b�O���`��
	//debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	////���G�͈͂��f�o�b�O�~���`��
	//debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	////�U���͈͂��f�o�b�O�~���`��
	//debugRenderer->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

//�꒣��ݒ�
void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
       
//�^�[�Q�b�g�ʒu������z�_���ɐݒ�
void EnemySlime::SetRandomTargetPosition()
{
	float targetRandomX{};//X�������_���l
	float targetRandomZ{};//Z�������_���l
	if (position.x != targetPosition.x
		&& position.y != targetPosition.y
		&& position.z != targetPosition.z)
	{
		targetRandomX = Mathf::RandomRange(-5.0f, 5.0f);
		targetRandomZ = Mathf::RandomRange(-5.0f, 5.0f);
	}
	targetPosition.x = position.x + targetRandomX;
	targetPosition.y = 0.5f;
	targetPosition.z = position.z + targetRandomZ;
}

//�ڕW�n�_�ֈړ�
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
	//�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	//�ړ�����
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool EnemySlime::SearchPlayer()
{
	//�v���C���[�Ƃ̍��፷���l������3D�ł̋������������
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
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
void EnemySlime::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
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
void EnemySlime::TransitionWanderState()
{
	state = State::Wander;

	//�ڕW�n�_�ݒ�
	SetRandomTargetPosition();

	//�����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_WalkFWD, true);
}

//�p�j�X�e�[�g�X�V����
void EnemySlime::UpdateWanderState(float elapsedTime)
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

	//�v���C���[���G
	if (SearchPlayer())
	{
		//����������ǐՃX�e�[�g�֑J��
		TransitionPursuitState();
	}
}

//�ҋ@�X�e�[�g�֑J��
void EnemySlime::TransitionIdleState()
{
	state = State::Idle;

	//�^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	//�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_IdleNormal, true);
}

//�ҋ@�X�e�[�g�X�V����
void EnemySlime::UpdateIdleState(float elapsedTime)
{
	//�^�C�}�[����
	stateTimer -= elapsedTime;

	if (stateTimer < 0.0f)
	{
		//�p�j�X�e�[�g�֑J��
		TransitionWanderState();
	}
}

//�ǐՃX�e�[�g�֑J��
void EnemySlime::TransitionPursuitState()
{
	state = State::Pursuit;

	//���b�ԒǐՂ���^�C�}�[�������_���ݒ�
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	//�����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_RunFWD, true);
}

//�ǐՃX�e�[�g�X�V����
void EnemySlime::UpdatePursuitState(float elapsedTime)
{
	//�ڕW�ʒu���v���C���[�̈ʒu�ŏ㏑��
	targetPosition = Player::Instance().GetPosition();

	//�ڕW�n�_�ֈړ�
	MoveToTarget(elapsedTime, 1.0f);
	SetTerritory(position, 10.0f);

	//�^�C�}�[����
	if (!SearchPlayer())
	{
		stateTimer -= elapsedTime;
	}

	if (stateTimer < 0.0f)
	{
		//�ҋ@�X�e�[�g�֑J��
		TransitionIdleState();
	}

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < attackRange)
	{
		//�U���X�e�[�g�֑J��
		TransitionAttackState();
	}
}

//�U���X�e�[�g�֑J��
void EnemySlime::TransitionAttackState()
{
	state = State::Attack;

	//�U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack1, false);
}

//�U���X�e�[�g�X�V����
void EnemySlime::UpdateAttackState(float elapsedTime)
{
	//�C�ӂ̃A�j���[�V������Ԃł̂ݏՓ˔��菈��������
	float animationTime = model->GetCurrentAnimetionSeconds();
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		//�ڋʂ̃m�[�h�ƃv���C���[�̏Փ˔���
		CollisionNodeVsPlayer("EyeBall", 0.2f);
	}
	//�U���A�j���[�V�������I��������퓬�ҋ@
	if (!model->IsPlayAnimation())
	{
		TransitionIdleBattleState();
	}
}

//�퓬�ҋ@�X�e�[�g�֑J��
void EnemySlime::TransitionIdleBattleState()
{
	state = State::IdleBattle;

	//���b�ԑҋ@����^�C�}�[�������_���Őݒ�
	stateTimer = Mathf::RandomRange(2.0f, 3.0f);

	//�퓬�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_IdleBattle, true);
}

//�퓬�ҋ@�X�e�[�g�X�V����
void EnemySlime::UpdateIdleBattleState(float elapsedTime)
{
	//�ڕW�n�_���v���C���[�ɐݒ�
	targetPosition = Player::Instance().GetPosition();

	//�^�C�}�[����
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		//�v���C���[���U���͈͂ɂ���΍U���X�e�[�g�֑J��
		float vx = targetPosition.x - position.x;
		float vy = targetPosition.y - position.y;
		float vz = targetPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		if (dist < attackRange)
		{
			//�U���X�e�[�g�֑J��
			TransitionAttackState();
		}
		else
		{
			//�p�j�X�e�[�g�֑J��
			TransitionWanderState();
		}
	}
	MoveToTarget(elapsedTime, 0.0f);
}

//�_���[�W�X�e�[�g�֑J��
void EnemySlime::TransitionDamageState()
{
	state = State::Damage;

	//�_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_GetHit, false);
}

//�_���[�W�X�e�[�g�X�V����
void EnemySlime::UpdateDamageState(float elapsedTime)
{
	//�_���[�W�A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionIdleBattleState();
	}
}

//���S�X�e�[�g�֑J��
void EnemySlime::TransitionDeathState()
{
	state = State::Death;

	//���S�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Die, false);
}

//���S�X�e�[�g�X�V����
void EnemySlime::UpdateDeathState(float elapsedTime)
{
	//�A�j���[�V�������I������玩�M��j��
	if (!model->IsPlayAnimation())
	{
		Destoroy();
	}
}