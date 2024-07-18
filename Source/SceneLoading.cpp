#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

//初期化
void SceneLoading::Initialize()
{
	//スプライト初期化
	//スレッド開始(第1引数に動かしたい処理、第2引数に関数の第1引数)
	thread = new std::thread(LoadingThread, this);
}

//終了化
void SceneLoading::Finalize()
{
	//スレッド終了化
	if (thread != nullptr)
	{
		//スレッド終了まで待機
		thread->join();
		//スレッドを破棄
		delete thread;
		//スレッドにnullを入れる
		thread = nullptr;
	}
}

//更新処理
void SceneLoading::Update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	//次のシーンの準備が完了したらシーンを切り替える
	//次のシーンのreadyがtrueかどうかチェック
	if (nextScene->IsReady() != false)
	{
		//シーンマネージャーでnextSceneに切り替える
		SceneManager::Instance().ChangeScene(nextScene);
		//nextSceneにnullを入れる
		nextScene = nullptr;
	}
}

//描画処理
void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->nextScene->Initialize();

	//スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	//次のシーンの準備完了設定
	scene->nextScene->SetReady();
}