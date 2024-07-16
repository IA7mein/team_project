#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Effect.h"

//�G�l�~�[
class Enemy : public Character
{
public:
	Enemy();
	~Enemy()override;

	//�C���X�^���X�擾
	static Enemy& Instance();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//�f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();

	//�꒣��ݒ�
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	//���͏��擾
	float GetPower() const { return power; };

	bool GetJump() const { return jumpFlag; };

	void eJump();

	//�^�[�Q�b�g�ʒu�������_���ɐݒ�
	void SetRandomTargetPosition();

protected:
	//���n�����ۂɌĂ΂��
	void OnLanding() override;

	////���S�����Ƃ��ɌĂ΂��
	//void OnDead() override;

	////�_���[�W���󂯂��ۂɌĂ΂��
	//void OnDamaged() override;

	void CollisionPlayerVsEnemies();

private:
	//�ڕW�n�_�ֈړ�
	void MoveToTarget(float elapsedTime, float speedRate);

	//�v���C���[���G
	bool SearchPlayer();

	//�m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	//�p�j�X�e�[�g�֑J��
	void TransitionWanderState();

	//�p�j�X�e�[�g�X�V����
	void UpdateWanderState(float elapsedTime);

	//�ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	////�ǐՃX�e�[�g�֑J��
	//void TransitionPursuitState();

	////�ǐՃX�e�[�g�X�V����
	//void UpdatePursuitState(float elapsedTime);

	////�U���X�e�[�g�֑J��
	//void TransitionAttackState();

	////�U���X�e�[�g�X�V����
	//void UpdateAttackState(float elapsedTime);

	////�퓬�ҋ@�X�e�[�g�֑J��
	//void TransitionIdleBattleState();

	////�퓬�ҋ@�X�e�[�g�֍X�V����
	//void UpdateIdleBattleState(float elapsedTime);

	////�_���[�W�X�e�[�g�֑J��
	//void TransitionDamageState();

	////�_���[�W�X�e�[�g�X�V����
	//void UpdateDamageState(float elapsedTime);

	////���S�X�e�[�g�֑J��
	//void TransitionDeathState();

	////���S�X�e�[�g�X�V����
	//void UpdateDeathState(float elapsedTime);

private:
	//�X�e�[�g
	enum class State
	{
		Wander,
		Idle,
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death
	};
	Model* model = nullptr;
	State state = State::Wander;
	DirectX::XMFLOAT3 targetPosition = { 1, 0, -7 };
	DirectX::XMFLOAT3 territoryOrigin = { 0, 0, 0};
	float territoryRange = 10.0f;
	float moveSpeed = 0.0f;
	float maxSpeed = 2.5f;
	float Velocity = 0.5f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 3.0f;
	float attackRange = 1.5f;
	int targetRandomX = 2;//X�������_���l
	int jumpCount = 0;
	int jumpLimit = 1;
	float jumpSpeed = 20.0f;
	bool jumpFlag = false;
	float jumpCT = 0.0f;
	float power = 0.0f;

	//�I�[�f�B�I���\�[�X
	std::unique_ptr<AudioSource>JumpStart;//�W�����v���͎���SE
	std::unique_ptr<AudioSource>JumpEnd;//���nSE
	std::unique_ptr<AudioSource>Hit;//�G�ɓ��������ۂ�SE
	std::unique_ptr<AudioSource>ItemGet;//�A�C�e���A�X�R�A����SE
	std::unique_ptr<AudioSource>HitObject;//��Q���q�b�gSE
};