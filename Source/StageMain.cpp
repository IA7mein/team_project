#include "StageMain.h"

//コンストラクタ
StageMain::StageMain()
{
	//ステージモデルを読み込み
	model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
	QuadtreeNode::CreateQuadtree({ 30.5f, 5.0f , 0.0f }, 65.0f, segmentDepth, &stageNodesQuadtree);

	int count = 0;
	// モデルのメッシュの三角形を四分割空間に登録
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュノード取得
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// レイをワールド空間からローカル空間へ変換
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		// 三角形（面）との交差判定
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// 三角形の頂点を抽出
				const ModelResource::Vertex& a = vertices.at(indices.at(index));
				const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				const DirectX::XMVECTOR A = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&a.position), WorldTransform);
				const DirectX::XMVECTOR B = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&b.position), WorldTransform);
				const DirectX::XMVECTOR C = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&c.position), WorldTransform);

				Triangle triangle = {};
				DirectX::XMStoreFloat3(&triangle.position[0], A);
				DirectX::XMStoreFloat3(&triangle.position[1], B);
				DirectX::XMStoreFloat3(&triangle.position[2], C);
				triangle.materialIndex = subset.materialIndex;

				// ワールド座標に変換した三角形を四分割空間に登録
				QuadtreeNode::InsertTriangleObject(stageNodesQuadtree, triangle, segmentDepth);
				count++;
			}
		}
	}
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
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
#if USE_QUADTREE == 1
	return QuadtreeNode::IntersectVerticalRayVsTriangle(&stageNodesQuadtree, segmentDepth, start, end, hit);
#else
	return Collision::IntersectRayVsModel(start, end, model, hit);
#endif
}