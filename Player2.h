#pragma once
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Effect.h"

//�v���C���[
class Player2 :public Character
{
public:
	Player2();
	~Player2() override;

	//�C���X�^���X�擾
	static Player2& Instance();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

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
	void CollisionPlayerVsPlayer();

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
extern  bool goalP2;