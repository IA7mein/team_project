#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "Enemy.h"
#include "Collision.h"
#include "SceneTitle.h"
static Player* instance = nullptr;
bool goal;
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

	model = new Model("Data/Model/Player/1P.mdl");

	JumpStart = Audio::Instance().LoadAudioSource("Data/SE/JumpStart.wav");
	JumpEnd = Audio::Instance().LoadAudioSource("Data/SE/JumpEnd.wav");
	Hit = Audio::Instance().LoadAudioSource("Data/SE/Hit.wav");
	ItemGet = Audio::Instance().LoadAudioSource("Data/SE/ItemGet.wav");
	HitObject = Audio::Instance().LoadAudioSource("Data/SE/HitObject.wav");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.1f;

	position.y = 5.4f;

	//�q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect("Data/Effect/damage.efk");
	hart = new Effect("Data/Effect/efec1.efk");
	shield = new Effect("Data/Effect/efec3.efk");
	goal = false;
	//�ҋ@�X�e�[�g�֑J��
	TransitionIdleState();
}

//�f�X�g���N�^
Player::~Player()
{
	JumpStart->Stop();
	JumpEnd->Stop();
	Hit->Stop();
	ItemGet->Stop();
	HitObject->Stop();

	delete model;

	delete hitEffect;
	delete hart;
	delete shield;
}

//�X�V����
void Player::Update(float elapsedTime)
{
	if (position.y < -10.0f)position = { 0, 1, -10 };

	//if(�S�[���ɂ�����)
	{
		goal = true;
	}

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
	/*case State::JumpFlip:
		UpdateJumpFlipState(elapsedTime);
		break;*/
	}
	
	//���͍X�V����
	UpdateVelocity(elapsedTime);

	//�v���C���[�ƓG�̏Փ˔���
	if(muluchmode==false)CollisionPlayerVsEnemies();

	//�v���C���[�ƃv���C���[�̏Փ˔���
	if (muluchmode)//������;

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
			ImGui::InputFloat("Power", &power);

			/*float enemypower = Enemy::Instance().GetPower();
			ImGui::InputFloat("EnemyPower", &enemypower);
			ImGui::Checkbox("jumpFlag", &jumpFlag);
			bool enemyjump = Enemy::Instance().GetJump();
			ImGui::Checkbox("EnemyjumpFlag", &enemyjump);*/
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
	//�i�s�x�N�g�����v�Z����
	if (ax < 0.0f)
	{
		ax = -1.0f;
	}
	else if (ax > 0.0f)
	{
		ax = 1.0f;
	}
	DirectX::XMFLOAT3 vec;
	vec.x = position.x + ax;
	vec.z = position.z + 1.0f;
	//Y�������ɂ͔������Ȃ�
	vec.y = 0.0f;

	return vec;
}

//�ړ����͏���
bool Player::InputMove(float elapsedTime)
{
	if (jumpCT > 0.0f)
	{
		jumpCT -= 1.0f * elapsedTime;
	}
	moveSpeed += Velocity * elapsedTime;
	if (moveSpeed >= maxSpeed && jumpFlag == false)
	{
		moveSpeed = maxSpeed;
	}

	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	float vx = moveVec.x - position.x;
	float vz = moveVec.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	if (vx != 0.0f && moveSpeed >= maxSpeed)
	{
		moveSpeed = 3.1f;
	}
	
	//�i�s�����ɉ����ĉ���������ݒ�
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
	Enemy& enemy = Enemy::Instance();
	//�Փˏ���
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		enemy.GetPosition(),
		enemy.GetRadius(),
		enemy.GetHeight(),
		outPosition
	))
	{
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy.GetPosition());
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

			if (jumpFlag && !enemy.GetJump())
			{
				Hit->Play(false);//�U���̃q�b�g���Đ�
				outPosition.x = position.x + (2.0f * L);
				enemy.SetPosition(outPosition);
			}
			else if (power != 0.0f && enemy.GetPower() * -1.0f == power)
			{
				outPosition.x = position.x + ((enemy.GetRadius() * 0.5f) * L);
				enemy.SetPosition(outPosition);
			}
			else if (power != 0.0f && enemy.GetPower() == 0.0f)
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				enemy.SetPosition(outPosition);
			}
			else
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				enemy.SetPosition(outPosition);
			}
		}
	}
}

//�W�����v���͏���
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisLY();
	if (ay >= 0.1f)
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
			if (muluchmode != true)
			{
				DirectX::XMFLOAT3 Epos = Enemy::Instance().GetPosition();
				if (position.x - Epos.x > 0.0f)
				{
					power = 1.0f;
				}
				else if (position.x - Epos.x < 0.0f)
				{
					power = -1.0f;
				}
			}
			//�W�����v���͂���
			return true;
		}		
	}
	return false;
}

//���n�����ۂɌĂ΂��
void Player::OnLanding()
{
	if (jumpFlag == true)
	{
		JumpEnd->Play(false);
		jumpCT = 1.0f;
		jumpFlag = false;
		jumpCount = 0;
		if (moveSpeed >= 1.5f)
		{
			moveSpeed /= 2.0f;
		}
	}

	if (velocity.y < -10.0f)
	{
		if (state != State::Damage && state != State::Death)
		{
			TransitionMoveState();			
		}
	}
}

//�U�����͏���
//bool Player::InputAttack()
//{
//	GamePad& gamePad = Input::Instance().GetGamePad();
//	float ay = gamePad.GetAxisLY();
//	if(ay > 0.1f)
//	{
//		return true;
//	}
//	return false;
//}

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

	////�W�����v���͏���
	//if (InputJump())
	//{
	//	TransitionJumpFlipState();
	//}
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

////2�i�W�����v�X�e�[�g�֑J��
//void Player::TransitionJumpFlipState()
//{
//	state = State::JumpFlip;
//
//	//�W�����v�A�j���[�V�����Đ�
//	model->PlayAnimation(Anim_Jump_Flip, false);
//}
//
////2�i�W�����v�X�e�[�g�X�V����
//void Player::UpdateJumpFlipState(float elapsedTime)
//{
//	InputMove(elapsedTime);
//}
//
////�U���X�e�[�g�֑J��
//void Player::TransitionAttackState()
//{
//	state = State::Attack;
//
//	//�U���A�j���[�V�����Đ�
//	//model->PlayAnimation(Anim_Attack, false);
//}
//
////�U���X�e�[�g�X�V����
//void Player::UpdateAttackState(float elapsedTime)
//{
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleState();
//	}
//	//�C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݓ����蔻��𔭐�������
//	float animetionTime = model->GetCurrentAnimetionSeconds();
//	if (animetionTime > 0.3f && animetionTime < 0.4f)
//	{
//		attackColisionFlag = true;
//	}
//	else attackColisionFlag = false;
//
//	if (attackColisionFlag)
//	{
//		//����m�[�h�ƃG�l�~�[�̏Փˏ���
//		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
//	}
//}
//
////�m�[�h�ƓG�̏Փˏ���
//void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
//{
//	//�m�[�h�擾
//	Model::Node* node = model->FindNode(nodeName);
//	//�m�[�h�ʒu�擾
//	DirectX::XMFLOAT3 nodePosition;
//	nodePosition.x = node->worldTransform._41;
//	nodePosition.y = node->worldTransform._42;
//	nodePosition.z = node->worldTransform._43;
//
//	Enemy& enemy = Enemy::Instance();
//
//	//�Փˏ���
//	DirectX::XMFLOAT3 outPosition;
//	if (Collision::IntersectSpherVsCyling(
//		nodePosition,
//		nodeRadius,
//		enemy.GetPosition(),
//		enemy.GetRadius(),
//		enemy.GetHeight(),
//		outPosition
//	))
//		//�_���[�W��^����
//		if (enemy.ApplyDamage(1, 0.5f))
//		{
//			//������΂�
//			{
//				DirectX::XMFLOAT3 impulse;
//				const float power = 10.0f;//������΂���
//
//				//�G�̈ʒu���擾
//				const DirectX::XMFLOAT3& e = enemy.GetPosition();
//				//�e�̈ʒu���擾
//				const DirectX::XMFLOAT3& p = nodePosition;
//				//�e����G�ւ̃x�N�g�����擾
//				float vx = e.x - p.x;
//				float vz = e.z - p.z;
//				//���̃x�N�g���𐳋K��(�������v�Z���A�����Ŋ���)
//				float lengthXZ = sqrtf(vx * vx + vz * vz);
//				vx /= lengthXZ;
//				vz /= lengthXZ;
//				//�Ռ��̒l��ݒ�(xz�͐��K�������x�N�g����power���X�P�[�����O)
//				impulse.x = vx * power;
//				impulse.y = power * 0.5f;
//				impulse.z = vz * power;
//
//				enemy.AddImpulse(impulse);
//			}
//			//�q�b�g�G�t�F�N�g�Đ�
//			{
//				DirectX::XMFLOAT3 e = enemy.GetPosition();
//				e.y += enemy.GetHeight() * 0.5f;
//				hitEffect->Play(e);
//			}
//		}
//}
//
////�_���[�W�X�e�[�g�֑J��
//void Player::TransitionDamageState()
//{
//	state = State::Damage;
//
//	//�_���[�W�A�j���[�V�����Đ�
//	model->PlayAnimation(Anim_GetHit1, false);
//}
//
////�_���[�W�X�e�[�g�X�V����
//void Player::UpdateDamageState(float elapsedTime)
//{
//	//�_���[�W�A�j���[�V�����I����ɑҋ@�X�e�[�g�֑J��
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleState();
//	}
//}
//
////���S�X�e�[�g�֑J��
//void Player::TransitionDeathState()
//{
//	state = State::Death;
//
//	//�_���[�W�A�j���[�V�����Đ�
//	model->PlayAnimation(Anim_Death, false);
//}
//
////���S�X�e�[�g�X�V����
//void Player::UpdateDeathState(float elapsedTime)
//{
//	if (!model->IsPlayAnimation())
//	{
//		//�{�^�����������畜��
//		GamePad& gamePad = Input::Instance().GetGamePad();
//		if (gamePad.GetButtonDown() & GamePad::BTN_A)
//		{
//			TransitionReviveState();
//		}
//	}
//}
//
////�����X�e�[�g�֑J��
//void Player::TransitionReviveState()
//{
//	state = State::Revive;
//
//	//�����A�j���[�V�����Đ�
//	model->PlayAnimation(Anim_Revive, false);
//}
//
////�����X�e�[�g�X�V����
//void Player::UpdateReviveState(float elapsedTime)
//{
//	//�����A�j���[�V�����I����ɑҋ@�X�e�[�g�֑J��
//	if (!model->IsPlayAnimation())
//	{
//		health = 5;
//		invincibleTimer = 1.0f;
//		TransitionIdleState();
//	}
//}