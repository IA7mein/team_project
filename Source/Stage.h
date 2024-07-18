#pragma once

#include "Graphics/Shader.h"
#include "Collision.h"

//ステージ
class Stage
{
public:
	Stage(){}
	virtual ~Stage(){}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	
	
};