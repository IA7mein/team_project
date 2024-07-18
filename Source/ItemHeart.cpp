#include "ItemHeart.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

//�R���X�g���N�^
ItemHeart::ItemHeart()
{
	model = new Model("Data/Model/Item/hart.mdl");

	//���f���̃X�P�[�����O
	scale.x = scale.y = scale.z = 1.0f;

	//���A�����ݒ�
	radius = 5.0f;
	height = 1.5f;
}

//�f�X�g���N�^
ItemHeart::~ItemHeart()
{
	delete model;
}

//�X�V����
void ItemHeart::Update(float elapsedTime)
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
void ItemHeart::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//�_���[�W���󂯂��ۂɌĂ΂��
void ItemHeart::OnDamaged()
{
	
}

//���S�����Ƃ��ɌĂ΂��
void ItemHeart::OnDead()
{
	
}

//�f�o�b�O�v���~�e�B�u�`��
void ItemHeart::DrawDebugPrimitive()
{
	//���N���X�̃f�o�b�O�v���~�e�B�u�`��
	Item::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
}

//�꒣��ݒ�
void ItemHeart::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}