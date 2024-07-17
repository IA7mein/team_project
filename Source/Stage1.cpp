#include "Stage1.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Camera.h"
#include"Enemy.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"


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
	enemy = new Enemy();
	enemy->SetPosition({ 1.0f, 0.0f, -10.0f });
	text = new Sprite("Data/Font/font6.png");

	bgm = Audio::Instance().LoadAudioSource("Data/BGM/���A�����ւ�I�ǂ����悤�I�I.wav");
	bgm->Play(true);
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

void Stage1::Update(float elapsedTime)
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

	//�G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
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
		enemy->Render(dc, shader);
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

		enemy->DrawDebugPrimitive();
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