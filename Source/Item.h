#pragma once

#include "Graphics/Graphics.h"
#include "Character.h"

//エネミー
class Item : public Character
{
public:
	Item() {}
	~Item() override {}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	//破棄
	void Destoroy();

};