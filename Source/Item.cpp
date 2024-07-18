#include "Item.h"
#include "Graphics/Graphics.h"
#include "ItemManager.h"

void Item::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//�Փ˔���p�̃f�o�b�O���`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//�j��
void Item::Destoroy()
{
	//�G�}�l�[�W���[�̍폜�Ɏ������g�̃A�h���X��n��
	ItemManager::Instance().Remove(this);
}