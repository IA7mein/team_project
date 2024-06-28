#include <imgui.h>
#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
	StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);

	//�v���C���[������
	player = new Player();
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
	//�J�����R���g���[���[������
	cameraController = new CameraController();
	//�G�l�~�[������
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 50; ++i)
	{
		EnemySlime* slime = new EnemySlime();		
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		slime->SetTerritory(slime->GetPosition(), 10.0f);
		//���������G�l�~�[�X���C�����G�l�~�[�}�l�[�W���[�ɓo�^
		enemyManager.Register(slime);
	}
	//�Q�[�W�X�v���C�g
	gauge = new Sprite();
}

// �I����
void SceneGame::Finalize()
{
	//�Q�[�W�I����
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}

	//�G�l�~�[�I����
	EnemyManager::Instance().Clear();
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
	//�X�e�[�W�I����
	StageManager::Instance().Clear();
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	//�J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//�X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);
	//�v���C���[�X�V����
	player->Update(elapsedTime);
	//�G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);
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
		//�G�l�~�[�`��
		EnemyManager::Instance().Render(dc, shader);
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

		EnemyManager::Instance().DrawDebugPrimitive();
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�X�v���C�g�`��
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		//�v���C���[�f�o�b�O�`��
		player->DrawDebugGUI();
	}
}

//�G�l�~�[HP�Q�[�W�`��
void SceneGame::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	//�r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewPorts = 1;
	dc->RSGetViewports(&numViewPorts, &viewport);

	//�ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//�S�Ă̓G�̓��̏��HP�Q�[�W��`��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	//�S�Ă̓G�̓��̏��HP�Q�[�W��`��
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		worldPosition.y = enemy->GetHeight();
		DirectX::XMVECTOR EnemyWorldPosition = DirectX::XMLoadFloat3(&worldPosition);
		//���[���h���W���X�N���[�����W�ɕϊ�
		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			EnemyWorldPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		//�X�N���[�����W
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// HP�Q�[�W�̕`��ʒu�ƃT�C�Y���v�Z
		float barWidth = 30.0f; // HP�o�[�̕�
		float barHeight = 5.0f; // HP�o�[�̍���

		// HP�Q�[�W�̐F��ݒ�i��: HP��50%�����̏ꍇ�͐ԁA����ȊO�͗΁j
		float hpRatio = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());

		// HP�o�[�̕`��
		gauge->Render(dc,
			screenPosition.x - barWidth * 0.5f,
			screenPosition.y - barHeight,
			barWidth * hpRatio,
			barHeight,
			0, 0,
			static_cast<float>(gauge->GetTextureWidth()),
			static_cast<float>(gauge->GetTextureHeight()),
			0,
			1.0f, 0.0f, 0.0f, 1.0f);
	}

	//�}�E�X�J�[�\���ʒu�ɂ��G�z�u
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPositionA{};
		screenPositionA.x = static_cast<float> (mouse.GetOldPositionX());
		screenPositionA.y = static_cast<float> (mouse.GetOldPositionY());

		DirectX::XMVECTOR ScreenPositionA = DirectX::XMLoadFloat3(&screenPositionA);

		DirectX::XMFLOAT3 screenPositionB{};
		screenPositionB.x = static_cast<float> (mouse.GetOldPositionX());
		screenPositionB.y = static_cast<float> (mouse.GetOldPositionY());
		screenPositionB.z = 1.0f;

		DirectX::XMVECTOR ScreenPositionB = DirectX::XMLoadFloat3(&screenPositionB);
		//Z�l��0.0�̎��̃��[���h���W
		//�X�N���[�����W�����[���h���W�ɐݒ�		
		DirectX::XMVECTOR WorldPositionA = DirectX::XMVector3Unproject(
			ScreenPositionA,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);
		//Z�l��1.0�̎��̃��[���h���W
		DirectX::XMVECTOR WorldPositionB = DirectX::XMVector3Unproject(
			ScreenPositionB,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);

		DirectX::XMVECTOR Ray = DirectX::XMVectorSubtract(WorldPositionB, WorldPositionA);//�x�N�g���̒������߂�
		DirectX::XMVECTOR V = DirectX::XMVector3Normalize(Ray);//���K��
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(V);

		HitResult hitResult;
		DirectX::XMFLOAT3 start;
		DirectX::XMStoreFloat3(&start, WorldPositionA);
		DirectX::XMFLOAT3 end;
		DirectX::XMStoreFloat3(&end, WorldPositionB);
		//���C�L���X�g�œG�ǉ�
		bool hit = StageManager::Instance().RayCast(start, end, hitResult);
		if (hit)
		{
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(hitResult.position);//���C�L���X�g��̃|�W�V������ݒ�
			enemyManager.Register(slime);
		}
	}
}