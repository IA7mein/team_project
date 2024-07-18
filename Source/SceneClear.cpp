#include "SceneClear.h"
#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
void SceneClear::Initialize()
{
	//sprite = new Sprite("Data/Sprite/Title.png");
	text = new Sprite("Data/Font/font1.png");
}

void SceneClear::Finalize()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

void SceneClear::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}
}

void SceneClear::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	//2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		/*float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());*/

		if(!muluchmode)text->textout(dc, "Player1", 240, 120, 110, 90, 150, 150, 30, 30, 0, 1, 1, 1, 1);
		if (!muluchmode)text->textout(dc, "Win", 440, 400, 130, 110, 150, 150, 30, 30, 0, 1, 0, 0, 1);
	}

}