#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"

void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");

	//BGM,SE設定
	bgm = Audio::Instance().LoadAudioSource("Data/BGM/野良猫のワルツ.wav");
	bgm->Play(true);
	PushButtan = Audio::Instance().LoadAudioSource("Data/SE/PushButtan.wav");
	MoveCarsol = Audio::Instance().LoadAudioSource("Data/SE/MoveCarsol.wav");
}

void SceneTitle::Finalize()
{
	//BGM,SE再生終了
	bgm->Stop();
	PushButtan->Stop();
	MoveCarsol->Stop();

	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	//何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		  GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y;

	if (gamePad.GetButtonDown()& anyButton)
	{
		PushButtan->Play(false);
		scene_change = true;
	}

	if (scene_change)
	{
		scene_timer += 1.0f * elapsedTime;
		if (scene_timer >= 1.0f)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		}
	}
}

//描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		//タイトルスプライト描画
		sprite->Render(dc,
			0, 0, screenWidth, screenHeight,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);
	}
}
