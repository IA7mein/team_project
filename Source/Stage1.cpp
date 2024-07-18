#include "Stage1.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Camera.h"
#include"Enemy.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "SceneTitle.h"
#include <ItemManager.h>
#include <ItemHeart.h>
#include <hari.h>
#include "SceneManager.h"
#include "SceneClear.h"
#include "../ItemShield.h"

void Stage1::Initialize()
{
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain(0);
	stageManager.Register(stageMain);


	//�v���C���[������
	player = new Player();
	player->SetPosition({ -1.0f, 0.0f, -10.0f });
	//�J����������
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	player->SetPosition(DirectX::XMFLOAT3(0, 5, 10));
	//�J�����R���g���[���[������
	cameraController = new CameraController();
	if (muluchmode == false)//��l�p
	{
		enemy = new Enemy();
		enemy->SetPosition({ 1.0f, 0.0f, -10.0f });
	}
	//�n�[�g������
	ItemManager& itemManager = ItemManager::Instance();
	ItemHeart* heart[5];
	for (int i = 0; i < 5; i++) { heart[i] = new ItemHeart(); };
	heart[0]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 50.0f));
	heart[1]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 40.0f));
	heart[2]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 20.0f));
	heart[3]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, -30.0f));
	heart[4]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -20.0f));	
	for (int i = 0; i < 5; i++) { itemManager.Register(heart[i]); }
	//�Ƃ�������5
	hari* Hari[5];
	for (int i = 0; i < 5; i++) { Hari[i] = new hari(); };
	Hari[0]->SetPosition(DirectX::XMFLOAT3(20.0f, 0.0f, -20.0f));
	Hari[1]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	Hari[2]->SetPosition(DirectX::XMFLOAT3(20.0f, 0.0f, 30.0f));
	Hari[3]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 20.0f));
	Hari[4]->SetPosition(DirectX::XMFLOAT3(-20.0f, 0.0f, -30.0f));
	for (int i = 0; i < 5; i++) { itemManager.Register(Hari[i]); }
	//��������
	ItemShield* shield[3];
	for (int i = 0; i < 3; i++) { shield[i] = new ItemShield(); };
	shield[0]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 30.0f));
	shield[1]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 10.0f));
	shield[2]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -40.0f));
	for (int i = 0; i < 3; i++) { itemManager.Register(shield[i]); }

	text = new Sprite("Data/Font/font6.png");

	bgm = Audio::Instance().LoadAudioSource("Data/BGM/���A�����ւ�I�ǂ����悤�I�I.wav");
	bgm->Play(true);
	goal = false;
	gauge = new Sprite();
}

void Stage1::Finalize()
{
	bgm->Stop();
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	//�A�C�e���I����
	ItemManager::Instance().Clear();

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
	if (muluchmode == false)
	{
		if (enemy != nullptr)
		{
			delete enemy;
			enemy = nullptr;
		}
	}
	if (text != nullptr)
	{
		delete text;
		text = nullptr;
	}
	goal = false;
	//�X�e�[�W�I����
	StageManager::Instance().Clear();
}

void Stage1::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	target.y =6.0f;
	target.z = playerPos.z - 5.0f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//�X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);
	//�G�l�~�[AI�X�V����
	if (muluchmode==false)
	{
		enemy->Update(elapsedTime);
	}
	//�v���C���[�X�V����
	player->Update(elapsedTime);

	if (goal)//�S�[��������
	{
		//SceneManager::Instance().ChangeScene(new SceneClear);
	}
	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
	//�A�C�e���X�V����
	ItemManager::Instance().Update(elapsedTime);
}

void Stage1::Render()
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
		if (muluchmode == false)
		{
			enemy->Render(dc, shader);
		}
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
		if(muluchmode == false)enemy->DrawDebugPrimitive();
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);
		//�A�C�e���f�o�b�N�v���~�e�B�u�`��
		ItemManager::Instance().DrawDebugPrimitive();
		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		//�v���C���[�f�o�b�O�`��
		//player->DrawDebugGUI();
		text->textout(dc, "Test", 0, 0, 30, 30, 150, 150, 30, 30, 0, 1, 1, 1, 1);
	}
}