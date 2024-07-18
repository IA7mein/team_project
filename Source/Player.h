#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Effect.h"

//�v���C���[
class Player :public Character
{
public:
	Player();
	~Player() override;

	//�C���X�^���X�擾
	static Player& Instance();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//�W�����v���͏���
	bool InputJump();

	//���͏�Ԏ擾
	float GetPower() const { return power; }

	//�W�����v��Ԏ擾
	bool GetJump() const { return jumpFlag; };
	
protected:
	//���n�����ۂɌĂ΂��
	void OnLanding() override;

	//�X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;

	//�ړ����͏���
	bool InputMove(float elapsedTime);

	//�v���C���[�ƃG�l�~�[�̏Փ˔���
	void CollisionPlayerVsEnemies();

	////�_���[�W���󂯂��ۂɌĂ΂��
	//void OnDamaged() override;

	////���S�����ۂɌĂ΂��
	//void OnDead() override;

private:
	////�U�����͏���
	//bool InputAttack();

	//�ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	//�ړ��X�e�[�g�֑J��
	void TransitionMoveState();

	//�ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

	//�W�����v�X�e�[�g�֑J��
	void TransitionJumpState();

	//�W�����v�X�e�[�g�X�V����
	void UpdateJumpState(float elapsedTime);

	//���n�X�e�[�g�֑J��
	void TransitionLandState();

	//���n�X�e�[�g�X�V����
	void UpdateLandState(float elapsedTime);

	////2�i�W�����v�X�e�[�g�֑J��
	//void TransitionJumpFlipState();

	////2�i�W�����v�X�e�[�g�X�V����
	//void UpdateJumpFlipState(float elapsedTime);

	////�U���X�e�[�g�֑J��
	//void TransitionAttackState();

	////�U���X�e�[�g�X�V����
	//void UpdateAttackState(float elapsedTime);

	////�m�[�h�ƃG�l�~�[�̏Փ˔���
	//void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	////�_���[�W�X�e�[�g�֑J��
	//void TransitionDamageState();

	////�_���[�W�X�e�[�g�X�V����
	//void UpdateDamageState(float elapsedTime);

	////���S�X�e�[�g�֑J��
	//void TransitionDeathState();

	////���S�X�e�[�g�X�V����
	//void UpdateDeathState(float elapsedTime);

	////�����X�e�[�g�ւ̑J��
	//void TransitionReviveState();

	////�����X�e�[�g�X�V����
	//void UpdateReviveState(float elapsedTime);

private:
	//�X�e�[�g
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		JumpFlip,
		Attack,
		Damage,
		Death,
		Revive
	};
	//�A�j���[�V����
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_Jump,
		Anim_Jump_Flip,
		Anim_Landing,
		Anim_Revive,
		Anim_Running,
		Anim_Walking
	};

	Model* model = nullptr;
	float moveSpeed = 0.0f;
	float maxSpeed = 2.0f;
	float Velocity = 0.5f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 20.0f;
	int jumpCount = 0;
	int jumpLimit = 1;
	
	Effect* hitEffect = nullptr;
	Effect* hart = nullptr;
	Effect* shield = nullptr;
	State state = State::Idle;
	float leftHandRadius = 0.4f;
	bool attackColisionFlag = false;
	bool jumpFlag = false;
	float jumpCT = 0.0f;
	float power = 0.0f;

	//�I�[�f�B�I���\�[�X
	std::unique_ptr<AudioSource>JumpStart;//�W�����v����SE
	std::unique_ptr<AudioSource>JumpEnd;//���n��SE
	std::unique_ptr<AudioSource>Hit;//�G�̍U���ɓ��������ۂ�SE
	std::unique_ptr<AudioSource>ItemGet;//�A�C�e���A�X�R�A����SE
	std::unique_ptr<AudioSource>HitObject;//��Q���q�b�gSE
};