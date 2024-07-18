#include "StageSelect.h"
#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include"Stage1.h"
#include"Stage2.h"
#include"Stage3.h"
void StageSelect::Initialize()
{
	sprite = new Sprite("Data/Sprite/senntaku1.png");
	sprite2 = new Sprite("Data/Sprite/senntaku2.png");
	sprite3 = new Sprite("Data/Sprite/senntaku3.png");
	//BGM,SE設定
	bgm = Audio::Instance().LoadAudioSource("Data/BGM/のんびりお散歩.wav");
	bgm->Play(true);
	PushButtan = Audio::Instance().LoadAudioSource("Data/SE/PushButtan.wav");
	MoveCarsol = Audio::Instance().LoadAudioSource("Data/SE/MoveCarsol.wav");
	tablestage = 0;
}

void StageSelect::Finalize()
{
	bgm->Stop();
	PushButtan->Stop();
	MoveCarsol->Stop();
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
	if (sprite2 != nullptr)
	{
		delete sprite2;
		sprite2 = nullptr;
	}
	if (sprite3 != nullptr)
	{
		delete sprite3;
		sprite3 = nullptr;
	}
}


void StageSelect::Update(float elapsedTime)
{
	if (carsol_timer > 0.0f)
	{
		carsol_timer -= 1.0f * elapsedTime;
	}
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	if (carsol_timer <= 0.0f)
	{
		if (ax > 0.0f && scene_change != true)
		{
			MoveCarsol->Play(false);
			tablestage++;
			carsol_timer = 1.0f;
		}
		if (ax < 0.0f && scene_change != true)
		{
			MoveCarsol->Play(false);
			tablestage--;
			carsol_timer = 1.0f;
		}
	}
	if (tablestage < 0)tablestage = 2;
	if (tablestage > 2)tablestage = 0;
	switch (tablestage)
	{
	case 0:
		if (gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			muluchmode = false;
			PushButtan->Play(false);
			scene_change = true;
		}
		if (scene_change)
		{
			scene_timer += 1.0f * elapsedTime;
			if (scene_timer >= 1.0f)
			{
				//SceneManager::Instance().ChangeScene(new Stage1);
				SceneManager::Instance().ChangeScene(new SceneLoading(new Stage1));
			}
		}
		break;
	case 1:
		if (gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			muluchmode = false;
			PushButtan->Play(false);
			scene_change = true;
		}
		if (scene_change)
		{
			scene_timer += 1.0f * elapsedTime;
			if (scene_timer >= 1.0f)
			{
				//SceneManager::Instance().ChangeScene(new Stage2);
				SceneManager::Instance().ChangeScene(new SceneLoading(new Stage2));
			}
		}
		break;
	case 2:
		if (gamePad.GetButtonDown() & GamePad::BTN_X)
		{
			muluchmode = false;
			PushButtan->Play(false);
			scene_change = true;
		}
		if (scene_change)
		{
			scene_timer += 1.0f * elapsedTime;
			if (scene_timer >= 1.0f)
			{
				//SceneManager::Instance().ChangeScene(new Stage3);
				SceneManager::Instance().ChangeScene(new SceneLoading(new Stage3));
			}
		}
		break;
	}
}


void StageSelect::Render()
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
		switch (tablestage)
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
			sprite3->Render(dc,
				0, 0, screenWidth, screenHeight,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
			break;


		}

	
	}
	
}
