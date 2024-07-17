#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include"StageSelect.h"
#include"SceneRule.h"

#include <sstream>

bool muluchmode;

void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/1.png");
	sprite2 = new Sprite("Data/Sprite/2.png");
	spriterule = new Sprite("Data/Sprite/3.png");
	font = new Sprite("Data/Font/font0.png");
	
	mode=0;
	statemode = 0;
	
	
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
	if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT)mode++;
	if (gamePad.GetButtonDown() & GamePad::BTN_LEFT)mode--;
	if (gamePad.GetButtonDown() & GamePad::BTN_START)statemode++;
	if (mode < 0)mode = 0;
	if (mode > 2)mode = 2;
	switch (mode)
	{
	case 0://一人モード
		if (gamePad.GetButtonDown() & GamePad::BTN_START)
		{
			muluchmode = false;
			SceneManager::Instance().ChangeScene(new StageSelect);
			SceneManager::Instance().ChangeScene(new SceneLoading(new StageSelect));
		}
		break;
	case 1://二人モード
		if (gamePad.GetButtonDown() & GamePad::BTN_START)
		{
			muluchmode = true;
			SceneManager::Instance().ChangeScene(new StageSelect);
			SceneManager::Instance().ChangeScene(new SceneLoading(new StageSelect));
		}
		break;
	case 2://ルール
		if (gamePad.GetButtonDown() & GamePad::BTN_START)
		{
			SceneManager::Instance().ChangeScene(new SceneRule);
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneRule));
		}
		break;
		
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
		switch (mode)
		{
		case 0:
			sprite->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
			break;
		case 1:
			sprite2->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
			break;
		case 2:
			spriterule->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
			break;
		}
		//font->textout(dc, "ECC College of Computer & Multimedia", 10, 10, 160, 160, 1, 1, 1, 1);
		
	}
}
;