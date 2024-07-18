#include "StageMain.h"

//コンストラクタ
StageMain::StageMain(int s)
{
	//ステージモデルを読み込み
	switch (s)
	{
	case 0:
		model = new Model("Data/Model/ExampleStage/Stage1.mdl");
		break;
	case 1:
		model = new Model("Data/Model/ExampleStage/stage2.mdl");
		break;
	case 2:
		model = new Model("Data/Model/ExampleStage/stage3.mdl");
		break;
	}
	QuadtreeNode::CreateQuadtree({ 30.5f, 5.0f , 0.0f }, 65.0f, segmentDepth, &stageNodesQuadtree);

	
	

	int count = 0;
	
}

StageMain::~StageMain()
{
	//ステージモデルを破棄
	delete model;
}
//更新処理
void StageMain::Update(float elapsedTime)
{

}
//描画処理
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//レイキャスト
