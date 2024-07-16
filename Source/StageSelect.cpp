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
}

void StageSelect::Finalize()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}


void StageSelect::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
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


void StageSelect::Render()
{

}
