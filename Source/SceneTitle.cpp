#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include"Stage1.h"
#include"Stage2.h"
#include"Stage3.h"

bool multimode;
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");
	multimode = false;
	statemode = 0;
	tablestage = 0;
}

void SceneTitle::Finalize()
{
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
	switch (statemode)
	{
	case 0:
		if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT)multimode = true;
		if (gamePad.GetButtonDown() & GamePad::BTN_LEFT)multimode = false;
		if (gamePad.GetButtonDown() & GamePad::BTN_START)statemode++;
		break;
	case 1:
		//ステージ 選択
		if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT)tablestage++;
		if (gamePad.GetButtonDown() & GamePad::BTN_LEFT)tablestage--;
		if (tablestage < 0)tablestage = 0;
		if (tablestage > 2)tablestage = 2;
		switch (tablestage)
		{
		case 0:
			if (gamePad.GetButtonDown() & GamePad::BTN_START)
			{
				SceneManager::Instance().ChangeScene(new Stage1);
				SceneManager::Instance().ChangeScene(new SceneLoading(new Stage1));
			}
			break;
		case 1:
			if (gamePad.GetButtonDown() & GamePad::BTN_START)
			{
				SceneManager::Instance().ChangeScene(new Stage2);
				SceneManager::Instance().ChangeScene(new SceneLoading(new Stage2));
			}
			break;
		case 2:
			if (gamePad.GetButtonDown() & GamePad::BTN_START)
			{
				SceneManager::Instance().ChangeScene(new Stage3);
				SceneManager::Instance().ChangeScene(new SceneLoading(new Stage3));
			}
			break;
		}
	}
	timer++;
	//何かボタンを押したらゲームシーンへ切り替え
	/*const GamePadButton anyButton =
		  GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y;

	if (gamePad.GetButtonDown()& anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneGame);
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}*/

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
