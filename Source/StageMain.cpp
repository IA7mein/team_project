#include "StageMain.h"

//�R���X�g���N�^
StageMain::StageMain()
{
	//�X�e�[�W���f����ǂݍ���
	model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
	QuadtreeNode::CreateQuadtree({ 30.5f, 5.0f , 0.0f }, 65.0f, segmentDepth, &stageNodesQuadtree);

	int count = 0;
	// ���f���̃��b�V���̎O�p�`���l������Ԃɓo�^
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ���b�V���m�[�h�擾
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

		// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		// �O�p�`�i�ʁj�Ƃ̌�������
		const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// �O�p�`�̒��_�𒊏o
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

				// ���[���h���W�ɕϊ������O�p�`���l������Ԃɓo�^
				QuadtreeNode::InsertTriangleObject(stageNodesQuadtree, triangle, segmentDepth);
				count++;
			}
		}
	}
}

StageMain::~StageMain()
{
	//�X�e�[�W���f����j��
	delete model;
}
//�X�V����
void StageMain::Update(float elapsedTime)
{

}
//�`�揈��
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
#if USE_QUADTREE == 1
	return QuadtreeNode::IntersectVerticalRayVsTriangle(&stageNodesQuadtree, segmentDepth, start, end, hit);
#else
	return Collision::IntersectRayVsModel(start, end, model, hit);
#endif
}