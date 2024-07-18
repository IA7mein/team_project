#include <imgui.h>
#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include"ItemManager.h"
#include"ItemHeart.h"
#include "../ItemShield.h"

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain(1);
	stageManager.Register(stageMain);
	/*StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);*/

	//�v���C���[������
	player = new Player();
	player->SetPosition({ -1.0f, 0.0f, -10.0f });
	//�J����������
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 0, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 10, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	//�J�����R���g���[���[������
	cameraController = new CameraController();
	
	//�n�[�g������
	/*ItemManager& itemManager = ItemManager::Instance();
	ItemHeart* heart[17];
	for (int i = 0; i < 17; i++) { heart[i] = new ItemHeart(); }*/
	/*ItemShield* shield[17];
	for (int i = 0; i < 17; i++) { shield[i] = new ItemShield(); }*/
	//�X�e�[�W�P
	//heart[0]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 30.0f));
	//heart[0]->SetTerritory(heart[0]->GetPosition(), 10.0f);

	//heart[1]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 40.0f));
	//heart[1]->SetTerritory(heart[1]->GetPosition(), 10.0f);

	//heart[2]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 30.0f));
	//heart[2]->SetTerritory(heart[2]->GetPosition(), 10.0f);

	//heart[3]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 50.0f));
	//heart[3]->SetTerritory(heart[3]->GetPosition(), 10.0f);

	//heart[4]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, 50.0f));
	//heart[4]->SetTerritory(heart[4]->GetPosition(), 10.0f);

	//heart[5]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 10.0f));
	//heart[5]->SetTerritory(heart[5]->GetPosition(), 10.0f);

	//heart[6]->SetPosition(DirectX::XMFLOAT3(10.0f, 5.4f, 40.0f));
	//heart[6]->SetTerritory(heart[6]->GetPosition(), 10.0f);

	//�X�e�[�W�Q
	//heart[0]->SetPosition(DirectX::XMFLOAT3(10.0f, 5.4f, 30.0f));
	//heart[0]->SetTerritory(heart[0]->GetPosition(), 10.0f);

	//heart[1]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 40.0f));
	//heart[1]->SetTerritory(heart[1]->GetPosition(), 10.0f);

	//heart[2]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 40.0f));
	//heart[2]->SetTerritory(heart[2]->GetPosition(), 10.0f);

	//heart[3]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 30.0f));
	//heart[3]->SetTerritory(heart[3]->GetPosition(), 10.0f);

	//heart[4]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 50.0f));
	//heart[4]->SetTerritory(heart[4]->GetPosition(), 10.0f);

	//heart[5]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, 50.0f));
	//heart[5]->SetTerritory(heart[5]->GetPosition(), 10.0f);

	//heart[6]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 0.0f));
	//heart[6]->SetTerritory(heart[6]->GetPosition(), 10.0f);

	//heart[7]->SetPosition(DirectX::XMFLOAT3(10.0f, 5.4f, 40.0f));
	//heart[7]->SetTerritory(heart[7]->GetPosition(), 10.0f);

	//heart[8]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 20.0f));
	//heart[8]->SetTerritory(heart[8]->GetPosition(), 10.0f);

	//heart[9]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 10.0f));
	//heart[9]->SetTerritory(heart[9]->GetPosition(), 10.0f);

	//heart[10]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 10.0f));
	//heart[10]->SetTerritory(heart[10]->GetPosition(), 10.0f);

	//heart[11]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, 0.0f));
	//heart[11]->SetTerritory(heart[11]->GetPosition(), 10.0f);

	//heart[12]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, -10.0f));
	//heart[12]->SetTerritory(heart[12]->GetPosition(), 10.0f);

	//heart[13]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, -10.0f));
	//heart[13]->SetTerritory(heart[13]->GetPosition(), 10.0f);

	//heart[14]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, -10.0f));
	//heart[14]->SetTerritory(heart[14]->GetPosition(), 10.0f);

	//heart[15]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, -20.0f));
	//heart[15]->SetTerritory(heart[15]->GetPosition(), 10.0f);

	//heart[16]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 20.0f));
	//heart[16]->SetTerritory(heart[16]->GetPosition(), 10.0f);

	////���������n�[�g���A�C�e���}�l�[�W���[�ɓo�^
	//for (int i = 0; i < 17; i++) { itemManager.Register(heart[i]); }
	//for (int i = 0; i < 17; i++) { itemManager.Register(shield[i]); }
	//�Q�[�W�X�v���C�g
	
}

// �I����
void SceneGame::Finalize()
{
	bgm->Stop();
	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	//�v���C���[�I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	//�G�l�~�[AI�I����
	if (enemy != nullptr)
	{
		delete enemy;
		enemy = nullptr;
	}
	if (text != nullptr)
	{
		delete text;
		text = nullptr;
	}
	//�X�e�[�W�I����
	StageManager::Instance().Clear();
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	target.y = 5.0f;
	target.z = playerPos.z - 5.0f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//�X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);
	//�G�l�~�[AI�X�V����
	enemy->Update(elapsedTime);
	//�v���C���[�X�V����
	player->Update(elapsedTime);
	//�A�C�e���X�V����
	ItemManager::Instance().Update(elapsedTime);
	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	//�J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
	
	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		//�X�e�[�W�`��	
		StageManager::Instance().Render(dc, shader);
		//�v���C���[�`��
		player->Render(dc, shader);
		//�A�C�e���`��
		ItemManager::Instance().Render(dc, shader);
		shader->End(dc);
	}

	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		player->DrawDebugPrimitive();
		//�A�C�e���f�o�b�N�v���~�e�B�u�`��
		ItemManager::Instance().DrawDebugPrimitive();
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}
	// 2D�f�o�b�OGUI�`��
	{
		//�v���C���[�f�o�b�O�`��
		player->DrawDebugGUI();

		text->textout(dc, "Test", 0, 0, 30, 30, 150, 150, 30, 30, 0, 1, 1, 1, 1);
	}
}