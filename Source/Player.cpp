#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

static Player* instance = nullptr;

//�C���X�^���X�擾
Player& Player::Instance()
{
	return *instance;
}

//�R���X�g���N�^
Player::Player()
{
	//�C���X�^���X�|�C���^�ݒ�
	instance = this;

	model = new Model("Data/Model/Jammo/Jammo.mdl");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.01f;

	//�q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//�ҋ@�X�e�[�g�֑J��
	TransitionIdleState();
}

//�f�X�g���N�^
Player::~Player()
{
	delete model;

	delete hitEffect;
}

//�X�V����
void Player::Update(float elapsedTime)
{
	if (position.y < -10.0f)position = { 0, 1, 0 };
	
	//�X�e�[�g���̏���
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	case State::Jump:
		UpdateJumpState(elapsedTime);
		break;
	case State::Land:
		UpdateLandState(elapsedTime);
		break;
	case State::JumpFlip:
		UpdateJumpFlipState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	case State::Revive:
		UpdateReviveState(elapsedTime);
		break;
	}

	//���͍X�V����
	UpdateVelocity(elapsedTime);

	//�v���C���[�ƓG�̏Փ˔���
	CollisionPlayerVsEnemies();

	//�e�ۂƓG�̏Փ˔���
	CollisionProjectileVsEnemies();

	//�e�ۍX�V����
	projectileManager.Update(elapsedTime);

	//���G���ԍX�V����
	UpdateInvincibleTimer(elapsedTime);

	//�I�u�W�F�N�g�s����X�V
	UpdateTransform();

	//���f���A�j���[�V�����X�V����
	model->UpdateAnimation(elapsedTime);

	//���f���s��X�V
	model->UpdateTransform(transform);
}

//�`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	//�e�ە`�揈��
	projectileManager.Render(dc, shader);
}
//�f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiTreeNodeFlags_None))
	{
		//�g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//�ʒu
			ImGui::InputFloat3("Position", &position.x);
			//��]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//�X�P�[��
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}

//�f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//�Փ˔���p�Ƀf�o�b�O����`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	//�e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();

	//�U���Փ˗p�̍���m�[�h�̃f�o�b�O����`��
	if (attackColisionFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(
			leftHandBone->worldTransform._41,
			leftHandBone->worldTransform._42,
			leftHandBone->worldTransform._43),
			leftHandRadius,
			DirectX::XMFLOAT4(1, 0, 0, 1)
		);
	}
}

//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLengh = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLengh > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraRightX /= cameraRightLengh;
		cameraRightZ /= cameraRightLengh;
	}

	//�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLengh = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLengh > 0.0f)
	{
		//�P�ʃx�N�g����
		cameraFrontX /= cameraFrontLengh;
		cameraFrontZ /= cameraFrontLengh;
	}
	//�X�e�B�b�N�̐������͒l���J�����E���ɔ��f���A
	//�X�e�B�b�N�̐������͒l���J�����O���ɔ��f���A
	//�i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y�������ɂ͔������Ȃ�
	vec.y = 0.0f;

	return vec;
}

//�ړ����͏���
bool Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//�ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	//�i�s�x�N�g����0�łȂ��ꍇ���͂��ꂽ
	if (moveVec.x != 0.0f || moveVec.z != 0.0f)
	{
		return true;
	}
	else return false;
}

//�v���C���[�ƃG�l�~�[�Ƃ̏Փ˔���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			//�ォ�瓥�񂾏ꍇ�͏��W�����v����
			if (normal.y > 0.8f)
			{
				//���W�����v����
				Jump(jumpSpeed * 0.5f);
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				//�����o����̈ʒu�ݒ�
				enemy->SetPosition(outPosition);
			}
		}
	}
}

//�W�����v���͏���
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{		
		//�W�����v�񐔐���
		if (jumpCount < jumpLimit)
		{
			//�W�����v
			jumpCount++;
			Jump(jumpSpeed);
			//�W�����v���͂���
			return true;
		}		
	}
	return false;
}

//���n�����ۂɌĂ΂��
void Player::OnLanding()
{
	jumpCount = 0;

	if (velocity.y < -10.0f)
	{
		if (state != State::Damage && state != State::Death)
		{
			TransitionLandState();
		}
	}
}

//�_���[�W���󂯂��ۂɌĂ΂��
void Player::OnDamaged()
{
	//�_���[�W�X�e�[�g�֑J��
	TransitionDamageState();
}

//���S�����ۂɌĂ΂��
void Player::OnDead()
{
	//���S�X�e�[�g�֑J��
	TransitionDeathState();
}

void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//���i�e��
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = transform._31;
		dir.y = transform._32;
		dir.z= transform._33;
		//���ˈʒu
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y;
		pos.z = position.z;

		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}
	//�ǔ��e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//�O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y); //dir.x = 
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//�^�[�Q�b�g(�f�t�H���g�̓v���C���[�̑O��)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//��ԋ߂��̓G���^�[�Q�b�g
		//��ԉ����ꏊ��ݒ�
		float dist = FLT_MAX;
		//�G�̃}�l�[�W���[���擾
		EnemyManager& enemyManager = EnemyManager::Instance();
		//�G�̐����擾
		int enemyCount = enemyManager.GetEnemyCount();

		//��ԉ����G��G�̐������`�F�b�N
		for (int i = 0; i < enemyCount; i++)
		{
			//�G�Ƃ̋�������
			//i�Ԗڂ̓G���擾
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			//�v���C���[�̈ʒu��XMVECTOR�ɕϊ�
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);			
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			// �G�����ւ̃x�N�g�����v�Z
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			//���̒������v�Z��
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			//�P��̒l�ɂ���
			float d;
			DirectX::XMStoreFloat(&d, D);
			//���܂Ōv�Z���������̒��ň�ԏ����������Ȃ�
			if (d < dist)
			{
				dist = d;
				//target�̈ʒu���X�V(�G�̈ʒu��ݒ�)
				target = enemy->GetPosition();
				//target��y�������̍����ɂ��Ă���
				target.y += enemy->GetHeight() * 0.5f;
			}
		}
		//����
		ProjectileHoming* projectilehoming = new ProjectileHoming(&projectileManager);
		projectilehoming->Launch(dir, pos, target);
	}
}

//�e�ۂƓG�̏Փ˔���
void Player::CollisionProjectileVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//�S�Ă̒e�ۂƂ��ׂĂ̓G�𑍓�����ŏՓ˔���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; i++)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; j++)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//�Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSpherVsCyling
			(projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//�_���[�W��^����
				if(enemy->ApplyDamage(1, 0.5f))
				{
					//������΂�
					{
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;//������΂���

						//�G�̈ʒu���擾
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						//�e�̈ʒu���擾
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
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
						
						enemy->AddImpulse(impulse);
					}
					//�q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}
					//�e�۔j��
					projectile->Destroy();
				}
			}
		}
	}
}

//�U�����͏���
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}
	return false;
}

//�ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState()
{
	state = State::Idle;

	//�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Idle, true);
}

//�ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime)
{
	//�ړ����͏���
	//bool Move = InputMove(elapsedTime);
	if (InputMove(elapsedTime))
	{
		TransitionMoveState();
	}

	//�W�����v���͏���
	if (InputJump())
	{
		TransitionJumpState();
	}

	//�U�����͏���
	if (InputAttack())
	{
		TransitionAttackState();
	}

	//�e�ۓ��͏���
	InputProjectile();
}

void Player::TransitionMoveState()
{
	state = State::Move;

	//����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Running, true);
}

//�ړ��X�e�[�g�X�V����
void Player::UpdateMoveState(float elapsedTime)
{
	//bool Move = InputMove(elapsedTime);
	if (!InputMove(elapsedTime))
	{
		TransitionIdleState();	
	}

	//�W�����v���͏���
	if (InputJump())
	{
		TransitionJumpState();
	}

	//�U�����͏���
	if (InputAttack())
	{
		TransitionAttackState();
	}

	//�e�ۓ��͏���
	InputProjectile();
}

//�W�����v�X�e�[�g�֑J��
void Player::TransitionJumpState()
{
	state = State::Jump;

	//�W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Jump, false);
}

//�W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime)
{
	InputMove(elapsedTime);

	//�W�����v���͏���
	if (InputJump())
	{
		TransitionJumpFlipState();
	}
}

//���n�X�e�[�g�֑J��
void Player::TransitionLandState()
{
	state = State::Land;

	//�W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Landing, false);
}

//���n�X�e�[�g�X�V����
void Player::UpdateLandState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

//2�i�W�����v�X�e�[�g�֑J��
void Player::TransitionJumpFlipState()
{
	state = State::JumpFlip;

	//�W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Jump_Flip, false);
}

//2�i�W�����v�X�e�[�g�X�V����
void Player::UpdateJumpFlipState(float elapsedTime)
{
	InputMove(elapsedTime);
}

//�U���X�e�[�g�֑J��
void Player::TransitionAttackState()
{
	state = State::Attack;

	//�U���A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Attack, false);
}

//�U���X�e�[�g�X�V����
void Player::UpdateAttackState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
	//�C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݓ����蔻��𔭐�������
	float animetionTime = model->GetCurrentAnimetionSeconds();
	if (animetionTime > 0.3f && animetionTime < 0.4f)
	{
		attackColisionFlag = true;
	}
	else attackColisionFlag = false;

	if (attackColisionFlag)
	{
		//����m�[�h�ƃG�l�~�[�̏Փˏ���
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
}

//�m�[�h�ƓG�̏Փˏ���
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
	//�m�[�h�擾
	Model::Node* node = model->FindNode(nodeName);
	//�m�[�h�ʒu�擾
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	EnemyManager& enemyManager = EnemyManager::Instance();

	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//�Փˏ���
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSpherVsCyling(
			nodePosition,
			nodeRadius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
			//�_���[�W��^����
			if (enemy->ApplyDamage(1, 0.5f))
			{
				//������΂�
				{
					DirectX::XMFLOAT3 impulse;
					const float power = 10.0f;//������΂���

					//�G�̈ʒu���擾
					const DirectX::XMFLOAT3& e = enemy->GetPosition();
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

					enemy->AddImpulse(impulse);
				}
				//�q�b�g�G�t�F�N�g�Đ�
				{
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					hitEffect->Play(e);
				}
			}
	}
}

//�_���[�W�X�e�[�g�֑J��
void Player::TransitionDamageState()
{
	state = State::Damage;

	//�_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_GetHit1, false);
}

//�_���[�W�X�e�[�g�X�V����
void Player::UpdateDamageState(float elapsedTime)
{
	//�_���[�W�A�j���[�V�����I����ɑҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

//���S�X�e�[�g�֑J��
void Player::TransitionDeathState()
{
	state = State::Death;

	//�_���[�W�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Death, false);
}

//���S�X�e�[�g�X�V����
void Player::UpdateDeathState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		//�{�^�����������畜��
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			TransitionReviveState();
		}
	}
}

//�����X�e�[�g�֑J��
void Player::TransitionReviveState()
{
	state = State::Revive;

	//�����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Revive, false);
}

//�����X�e�[�g�X�V����
void Player::UpdateReviveState(float elapsedTime)
{
	//�����A�j���[�V�����I����ɑҋ@�X�e�[�g�֑J��
	if (!model->IsPlayAnimation())
	{
		health = 5;
		invincibleTimer = 1.0f;
		TransitionIdleState();
	}
}