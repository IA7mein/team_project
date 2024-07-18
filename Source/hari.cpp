#include "hari.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

//�R���X�g���N�^
hari::hari()
{
	model = new Model("Data/Model/Item/hari.mdl");

	//���f���̃X�P�[�����O
	scale.x = scale.y = scale.z = 0.05f;

	//���A�����ݒ�
	radius = 5.0f;
	height = 1.5f;
}

//�f�X�g���N�^
hari::~hari()
{
	delete model;
}

//�X�V����
void hari::Update(float elapsedTime)
{


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
void hari::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//�_���[�W���󂯂��ۂɌĂ΂��
void hari::OnDamaged()
{

}

//���S�����Ƃ��ɌĂ΂��
void hari::OnDead()
{

}

//�f�o�b�O�v���~�e�B�u�`��
void hari::DrawDebugPrimitive()
{
	//���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Item::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
}

//�꒣��ݒ�
void hari::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}