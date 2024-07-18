#include <imgui.h>
#include "Player.h"
#include "Player2.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "Collision.h"
#include "SceneTitle.h"
static Player2* instance = nullptr;
bool goalP2;
//�C���X�^���X�擾
Player2& Player2::Instance()
{
	return *instance;
}

//�R���X�g���N�^
Player2::Player2()
{
	//�C���X�^���X�|�C���^�ݒ�
	instance = this;

	model = new Model("Data/Model/Player/2P.mdl");

	JumpStart = Audio::Instance().LoadAudioSource("Data/SE/JumpStart.wav");
	JumpEnd = Audio::Instance().LoadAudioSource("Data/SE/JumpEnd.wav");
	Hit = Audio::Instance().LoadAudioSource("Data/SE/Hit.wav");
	ItemGet = Audio::Instance().LoadAudioSource("Data/SE/ItemGet.wav");
	HitObject = Audio::Instance().LoadAudioSource("Data/SE/HitObject.wav");

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.5f;

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
Player2::~Player2()
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
void Player2::Update(float elapsedTime)
{
	if (position.y < -10.0f)position = { 0, 1, -10 };

	if (position.z >= 78.0f)
	{
		goalP2 = true;
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

	//�v���C���[�ƃv���C���[�̏Փ˔���
	if (muluchmode)//������;
	{
		CollisionPlayerVsPlayer();
	}

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
void Player2::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//�f�o�b�O�v���~�e�B�u�`��
void Player2::DrawDebugPrimitive()
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
DirectX::XMFLOAT3 Player2::GetMoveVec() const
{
	//���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
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
bool Player2::InputMove(float elapsedTime)
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
	return false;

}

//�v���C���[�ƃG�l�~�[�Ƃ̏Փ˔���
void Player2::CollisionPlayerVsPlayer()
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

			if (power != 0)
			{
				if (jumpFlag && !player.GetJump())
				{
					Hit->Play(false);//�U���̃q�b�g���Đ�
					outPosition.x = position.x + (5.0f * L);
					player.SetPosition(outPosition);
				}
				else if (power != 0.0f && player.GetPower() * -1.0f == power)
				{
					outPosition.x = position.x + ((player.GetRadius()) * L);
					player.SetPosition(outPosition);
				}
				else if (power != 0.0f && player.GetPower() == 0.0f)
				{
					Hit->Play(false);
					outPosition.x = position.x + (5.0f * L);
					player.SetPosition(outPosition);
				}
				else
				{
					Hit->Play(false);
					outPosition.x = position.x + (5.0f * L);
					player.SetPosition(outPosition);
				}
			}
		}
	}
}

//�W�����v���͏���
bool Player2::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisRY();
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
			//�W�����v���͂���
			return true;
		}
	}
	return false;
}

//���n�����ۂɌĂ΂��
void Player2::OnLanding()
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

//�ҋ@�X�e�[�g�֑J��
void Player2::TransitionIdleState()
{
	state = State::Idle;

	//�ҋ@�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Idle, true);
}

//�ҋ@�X�e�[�g�X�V����
void Player2::UpdateIdleState(float elapsedTime)
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

void Player2::TransitionMoveState()
{
	state = State::Move;

	//����A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Running, true);
}

//�ړ��X�e�[�g�X�V����
void Player2::UpdateMoveState(float elapsedTime)
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
void Player2::TransitionJumpState()
{
	state = State::Jump;

	//�W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Jump, false);
}

//�W�����v�X�e�[�g�X�V����
void Player2::UpdateJumpState(float elapsedTime)
{
	InputMove(elapsedTime);

	////�W�����v���͏���
	//if (InputJump())
	//{
	//	TransitionJumpFlipState();
	//}
}

//���n�X�e�[�g�֑J��
void Player2::TransitionLandState()
{
	state = State::Land;

	//�W�����v�A�j���[�V�����Đ�
	model->PlayAnimation(Anim_Landing, false);
}

//���n�X�e�[�g�X�V����
void Player2::UpdateLandState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}
