#include <imgui.h>
#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"
#include "StageMoveFloor.h"
#include"ItemManager.h"
#include"ItemHeart.h"
#include "../ItemShield.h"

// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);
	/*StageMoveFloor* stageMoveFloor = new StageMoveFloor();
	stageMoveFloor->SetStartPoint(DirectX::XMFLOAT3(0, 1, 3));
	stageMoveFloor->SetGoalPoint(DirectX::XMFLOAT3(10, 2, 3));
	stageMoveFloor->SetTorque(DirectX::XMFLOAT3(0, 1.0f, 0));
	stageManager.Register(stageMoveFloor);*/

	//プレイヤー初期化
	player = new Player();
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
	
	//ハート初期化
	ItemManager& itemManager = ItemManager::Instance();
	ItemHeart* heart[17];
	for (int i = 0; i < 17; i++) { heart[i] = new ItemHeart(); }
	/*ItemShield* shield[17];
	for (int i = 0; i < 17; i++) { shield[i] = new ItemShield(); }*/
	//ステージ１
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

	//ステージ２
	heart[0]->SetPosition(DirectX::XMFLOAT3(10.0f, 5.4f, 30.0f));
	heart[0]->SetTerritory(heart[0]->GetPosition(), 10.0f);

	heart[1]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 40.0f));
	heart[1]->SetTerritory(heart[1]->GetPosition(), 10.0f);

	heart[2]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 40.0f));
	heart[2]->SetTerritory(heart[2]->GetPosition(), 10.0f);

	heart[3]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 30.0f));
	heart[3]->SetTerritory(heart[3]->GetPosition(), 10.0f);

	heart[4]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 50.0f));
	heart[4]->SetTerritory(heart[4]->GetPosition(), 10.0f);

	heart[5]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, 50.0f));
	heart[5]->SetTerritory(heart[5]->GetPosition(), 10.0f);

	heart[6]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, 0.0f));
	heart[6]->SetTerritory(heart[6]->GetPosition(), 10.0f);

	heart[7]->SetPosition(DirectX::XMFLOAT3(10.0f, 5.4f, 40.0f));
	heart[7]->SetTerritory(heart[7]->GetPosition(), 10.0f);

	heart[8]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 20.0f));
	heart[8]->SetTerritory(heart[8]->GetPosition(), 10.0f);

	heart[9]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, 10.0f));
	heart[9]->SetTerritory(heart[9]->GetPosition(), 10.0f);

	heart[10]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 10.0f));
	heart[10]->SetTerritory(heart[10]->GetPosition(), 10.0f);

	heart[11]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, 0.0f));
	heart[11]->SetTerritory(heart[11]->GetPosition(), 10.0f);

	heart[12]->SetPosition(DirectX::XMFLOAT3(-20.0f, 5.4f, -10.0f));
	heart[12]->SetTerritory(heart[12]->GetPosition(), 10.0f);

	heart[13]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, -10.0f));
	heart[13]->SetTerritory(heart[13]->GetPosition(), 10.0f);

	heart[14]->SetPosition(DirectX::XMFLOAT3(20.0f, 5.4f, -10.0f));
	heart[14]->SetTerritory(heart[14]->GetPosition(), 10.0f);

	heart[15]->SetPosition(DirectX::XMFLOAT3(0.0f, 5.4f, -20.0f));
	heart[15]->SetTerritory(heart[15]->GetPosition(), 10.0f);

	heart[16]->SetPosition(DirectX::XMFLOAT3(-10.0f, 5.4f, 20.0f));
	heart[16]->SetTerritory(heart[16]->GetPosition(), 10.0f);

	//生成したハートをアイテムマネージャーに登録
	for (int i = 0; i < 17; i++) { itemManager.Register(heart[i]); }
	//for (int i = 0; i < 17; i++) { itemManager.Register(shield[i]); }
	//ゲージスプライト
	gauge = new Sprite();
}

// 終了化
void SceneGame::Finalize()
{
	//ゲージ終了化
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
	//ステージ終了化
	StageManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//ステージ更新処理
	StageManager::Instance().Update(elapsedTime);
	//プレイヤー更新処理
	player->Update(elapsedTime);
	//アイテム更新処理
	ItemManager::Instance().Update(elapsedTime);
	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
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
		//アイテム描画
		ItemManager::Instance().Render(dc, shader);
		shader->End(dc);
	}

	//3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		//プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();
		//アイテムデバックプリミティブ描画
		ItemManager::Instance().DrawDebugPrimitive();
		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2DデバッグGUI描画
	{
		//プレイヤーデバッグ描画
		player->DrawDebugGUI();
	}
}

//エネミーHPゲージ描画
void SceneGame::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewPorts = 1;
	dc->RSGetViewports(&numViewPorts, &viewport);

	//変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	////全ての敵の頭の上にHPゲージを描画
	//EnemyManager& enemyManager = EnemyManager::Instance();
	//int enemyCount = enemyManager.GetEnemyCount();

	////全ての敵の頭の上にHPゲージを描画
	//for (int i = 0; i < enemyCount; ++i)
	//{
	//	Enemy* enemy = enemyManager.GetEnemy(i);

	//	DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
	//	worldPosition.y = enemy->GetHeight();
	//	DirectX::XMVECTOR EnemyWorldPosition = DirectX::XMLoadFloat3(&worldPosition);
	//	//ワールド座標をスクリーン座標に変換
	//	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
	//		EnemyWorldPosition,
	//		viewport.TopLeftX,
	//		viewport.TopLeftY,
	//		viewport.Width,
	//		viewport.Height,
	//		viewport.MinDepth,
	//		viewport.MaxDepth,
	//		Projection,
	//		View,
	//		World
	//	);

	//	//スクリーン座標
	//	DirectX::XMFLOAT2 screenPosition;
	//	DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

	//	// HPゲージの描画位置とサイズを計算
	//	float barWidth = 30.0f; // HPバーの幅
	//	float barHeight = 5.0f; // HPバーの高さ

	//	// HPゲージの色を設定（例: HPが50%未満の場合は赤、それ以外は緑）
	//	float hpRatio = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());

	//	// HPバーの描画
	//	gauge->Render(dc,
	//		screenPosition.x - barWidth * 0.5f,
	//		screenPosition.y - barHeight,
	//		barWidth * hpRatio,
	//		barHeight,
	//		0, 0,
	//		static_cast<float>(gauge->GetTextureWidth()),
	//		static_cast<float>(gauge->GetTextureHeight()),
	//		0,
	//		1.0f, 0.0f, 0.0f, 1.0f);
	//}

	////マウスカーソル位置による敵配置
	//Mouse& mouse = Input::Instance().GetMouse();
	//if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	//{
	//	//マウスカーソル座標を取得
	//	DirectX::XMFLOAT3 screenPositionA{};
	//	screenPositionA.x = static_cast<float> (mouse.GetOldPositionX());
	//	screenPositionA.y = static_cast<float> (mouse.GetOldPositionY());

	//	DirectX::XMVECTOR ScreenPositionA = DirectX::XMLoadFloat3(&screenPositionA);

	//	DirectX::XMFLOAT3 screenPositionB{};
	//	screenPositionB.x = static_cast<float> (mouse.GetOldPositionX());
	//	screenPositionB.y = static_cast<float> (mouse.GetOldPositionY());
	//	screenPositionB.z = 1.0f;

	//	DirectX::XMVECTOR ScreenPositionB = DirectX::XMLoadFloat3(&screenPositionB);
	//	//Z値が0.0の時のワールド座標
	//	//スクリーン座標をワールド座標に設定		
	//	DirectX::XMVECTOR WorldPositionA = DirectX::XMVector3Unproject(
	//		ScreenPositionA,
	//		viewport.TopLeftX,
	//		viewport.TopLeftY,
	//		viewport.Width,
	//		viewport.Height,
	//		viewport.MinDepth,
	//		viewport.MaxDepth,
	//		Projection,
	//		View,
	//		World
	//	);
	//	//Z値が1.0の時のワールド座標
	//	DirectX::XMVECTOR WorldPositionB = DirectX::XMVector3Unproject(
	//		ScreenPositionB,
	//		viewport.TopLeftX,
	//		viewport.TopLeftY,
	//		viewport.Width,
	//		viewport.Height,
	//		viewport.MinDepth,
	//		viewport.MaxDepth,
	//		Projection,
	//		View,
	//		World
	//	);

	//	DirectX::XMVECTOR Ray = DirectX::XMVectorSubtract(WorldPositionB, WorldPositionA);//ベクトルの長さ求める
	//	DirectX::XMVECTOR V = DirectX::XMVector3Normalize(Ray);//正規化
	//	DirectX::XMVECTOR Length = DirectX::XMVector3Length(V);

	//	HitResult hitResult;
	//	DirectX::XMFLOAT3 start;
	//	DirectX::XMStoreFloat3(&start, WorldPositionA);
	//	DirectX::XMFLOAT3 end;
	//	DirectX::XMStoreFloat3(&end, WorldPositionB);
	//	//レイキャストで敵追加
	//	bool hit = StageManager::Instance().RayCast(start, end, hitResult);
	//	if (hit)
	//	{
	//		Enemyheart* heart = new Enemyheart();
	//		heart->SetPosition(hitResult.position);//レイキャスト後のポジションを設定
	//		enemyManager.Register(heart);
	//	}
	//}
}