#pragma once

#include "Graphics/Model.h"
#include "Stage.h"

//ステージ
class StageMain : public Stage
{
public:
	StageMain(int s);
	~StageMain() override;

	//更新処理
	void Update(float elasedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	//レイキャスト
	//bool RayCast(const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end,
	//	HitResult& hit)override;

private:
	std::vector<QuadtreeNode>	stageNodesQuadtree;
	// 四分割階層数
	uint32_t segmentDepth = 4;
	Model* model = nullptr;
};

#define USE_QUADTREE 1