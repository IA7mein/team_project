#include "Stage3.h"
#include <imgui.h>
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "EffectManager.h"
#include"Enemy.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "SceneTitle.h"
#include "ItemManager.h"
#include "ItemHeart.h"
#include "hari.h"
#include "ItemShield.h"
#include "SceneManager.h"
#include "SceneClear.h"
void Stage3::Initialize()
{
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain(2);
	stageManager.Register(stageMain);


	//プレイヤー初期化
	player = new Player();
	player->SetPosition({ -5.0f, 0.0f, -58.0f });
	//カメラ初期化
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
	//カメラコントローラー初期化
	cameraController = new CameraController();
	if (muluchmode == false)//一人用
	{
		enemy = new Enemy();
		enemy->SetPosition({ 5.0f, 0.0f, -58.0f });
	}
	else if (muluchmode != false)
	{
		player2 = new Player2();
		player2->SetPosition({ 5.0f, 0.0f, -58.0f });
	}
	text = new Sprite("Data/Font/font6.png");
	//ハート初期化
	ItemManager& itemManager = ItemManager::Instance();
	ItemHeart* heart[6];
	for (int i = 0; i < 6; i++) { heart[i] = new ItemHeart(); };
	heart[0]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 30.0f));
	heart[1]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 40.0f));
	heart[2]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 20.0f));
	heart[3]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, -30.0f));
	heart[4]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -20.0f));
	heart[5]->SetPosition(DirectX::XMFLOAT3(20.0f, 0.0f, -10.0f));
	for (int i = 0; i < 6; i++) { itemManager.Register(heart[i]); }
	//とげ初期化
	hari* Hari[5];
	for (int i = 0; i < 5; i++) { Hari[i] = new hari(); };
	Hari[0]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 30.0f));
	Hari[1]->SetPosition(DirectX::XMFLOAT3(-20.0f, 0.0f, 30.0f));
	Hari[2]->SetPosition(DirectX::XMFLOAT3(20.0f, 0.0f, 10.0f));
	Hari[3]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f));
	Hari[4]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, -10.0f));
	for (int i = 0; i < 5; i++) { itemManager.Register(Hari[i]); }
	//盾初期化
	ItemShield* shield[3];
	for (int i = 0; i < 3; i++) { shield[i] = new ItemShield(); };
	shield[0]->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f));
	shield[1]->SetPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 30.0f));
	shield[2]->SetPosition(DirectX::XMFLOAT3(10.0f, 0.0f, -40.0f));
	for (int i = 0; i < 3; i++) { itemManager.Register(shield[i]); }

	bgm = Audio::Instance().LoadAudioSource("Data/BGM/これより開幕！.wav");
	bgm->Play(true);
	goal = false;
	gauge = new Sprite();
}

void Stage3::Finalize()
{
	bgm->Stop();
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}
	//アイテム終了化
	ItemManager::Instance().Clear();

	//カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	//プレイヤー終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	if (player2 != nullptr)
	{
		delete player2;
		player2 = nullptr;
	}
	//エネミーAI終了化
	if (enemy != nullptr && muluchmode == false)
	{
		delete enemy;
		enemy = nullptr;
	}
	if (text != nullptr)
	{
		delete text;
		text = nullptr;
	}
	goal = false;
	//ステージ終了化
	StageManager::Instance().Clear();
}

void Stage3::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	target.y = 25.0f;
	target.z = playerPos.z - 50.0f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//ステージ更新処理
	StageManager::Instance().Update(elapsedTime);
	//エネミーAI更新処理
	if (muluchmode == false)enemy->Update(elapsedTime);
	else if (muluchmode != false)
	{
		player2->Update(elapsedTime);
	}
	//プレイヤー更新処理
	player->Update(elapsedTime);
	if (goal || goalP2)//ゴールしたら
	{
		SceneManager::Instance().ChangeScene(new SceneClear);
	}

	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
	//アイテム更新処理
	ItemManager::Instance().Update(elapsedTime);
}

void Stage3::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		//ステージ描画	
		StageManager::Instance().Render(dc, shader);
		//プレイヤー描画
		player->Render(dc, shader);
		if (muluchmode == false)enemy->Render(dc, shader);
		else if (muluchmode != false)
		{
			player2->Render(dc, shader);
		}
		shader->End(dc);
	}

	//3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		////プレイヤーデバッグプリミティブ描画
		//player->DrawDebugPrimitive();

		//if (muluchmode == false)enemy->DrawDebugPrimitive();
		//// ラインレンダラ描画実行
		//graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		//// デバッグレンダラ描画実行
		//graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
		////アイテムデバックプリミティブ描画
		//ItemManager::Instance().DrawDebugPrimitive();
	}

	// 2DデバッグGUI描画
	{
		////プレイヤーデバッグ描画
		//player->DrawDebugGUI();
		//text->textout(dc, "Test", 0, 0, 30, 30, 150, 150, 30, 30, 0, 1, 1, 1, 1);
	}
}