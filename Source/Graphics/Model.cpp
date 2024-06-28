#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "ResourceManager.h"

// �R���X�g���N�^
Model::Model(const char* filename)
{
	// ���\�[�X�ǂݍ���
	//resource = std::make_shared<ModelResource>();
	//resource->Load(Graphics::Instance().GetDevice(), filename);
	resource = ResourceManager::Instance().LoadModelResource(filename);

	// �m�[�h
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// �s��v�Z
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// �ϊ��s��v�Z
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ���[�J���s��Z�o
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ���[���h�s��Z�o
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// �v�Z���ʂ��i�[
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}
}

//�A�j���[�V�����X�V����
void Model::UpdateAnimation(float elapsedTime)
{
	//�Đ����Ȃ甲����
	if (!IsPlayAnimation())return;

	//�u�����h���̌v�Z
	float blendRate = 1.0f;
	if (animationBlendSeconds > 0.0f)
	{
		animationBlendTime = currentAnimationSeconds;
		blendRate = animationBlendTime / animationBlendSeconds;		
	}

	//�w��̃A�j���[�V�����f�[�^���擾
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	//�A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		//���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩�v�Z
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentAnimationSeconds >= keyframe0.seconds &&
			currentAnimationSeconds < keyframe1.seconds)
		{
			//�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o
			float rate = (currentAnimationSeconds - keyframe0.seconds)
							/ (keyframe1.seconds);

			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				//2�̃L�[�t���[���Ԃ̕ۊǏ���
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				//�u�����h�ۊǏ���
				if (blendRate < 1.0f)
				{
					//���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕ۊ�
					DirectX::XMVECTOR current_Position = DirectX::XMLoadFloat3(&node.translate);
					DirectX::XMVECTOR current_Rotation = DirectX::XMLoadFloat4(&node.rotate);
					DirectX::XMVECTOR key1_Position = DirectX::XMLoadFloat3(&key1.translate);
					DirectX::XMVECTOR key1_Rotation = DirectX::XMLoadFloat4(&key1.rotate);

					DirectX::XMVECTOR Position = DirectX::XMVectorLerp(current_Position, key1_Position, blendRate);
					DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(current_Rotation, key1_Rotation, blendRate);
					DirectX::XMStoreFloat3(&node.translate, Position);
					DirectX::XMStoreFloat4(&node.rotate, Rotation);
				}
				//�ʏ�̌v�Z
				else
				{
					//�O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
					DirectX::XMVECTOR key0_Position = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR key0_Rotation = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR key1_Position = DirectX::XMLoadFloat3(&key1.translate);
					DirectX::XMVECTOR key1_Rotation = DirectX::XMLoadFloat4(&key1.rotate);

					DirectX::XMVECTOR Position = DirectX::XMVectorLerp(key0_Position, key1_Position, rate);
					DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(key0_Rotation, key1_Rotation, rate);
					DirectX::XMStoreFloat3(&node.translate, Position);
					DirectX::XMStoreFloat4(&node.rotate, Rotation);
				}

			}
			break;
		}
	}
	//�ŏI�t���[������
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}

	//�o�ߎ���
	currentAnimationSeconds += elapsedTime;

	//�Đ����Ԃ��I�[���Ԃ𒴂�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		
		if (animationLoopFlag == true)//���[�v��true�Ȃ�
		{
			//�Đ����Ԃ������߂�
			currentAnimationSeconds -= animation.secondsLength;
			animationEndFlag = false;
		}

		else
		{
			animationEndFlag = true;
		}
	}
}

//�A�j���[�V�����Đ�
void Model::PlayAnimation(int index, bool loop, float blendSecons)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0f;
	animationBlendSeconds = blendSecons;
}

//�A�j���[�V�����Đ�����
bool Model::IsPlayAnimation()const
{
	if (currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= resource->GetAnimations().size())return false;
	return true;
}

//�m�[�h����
Model::Node* Model::FindNode(const char* name)
{
	//�S�Ẵm�[�h�Ƒ�������Ŗ��O��r
	int nodeCount = static_cast<int>(nodes.size());
	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		//���݌��Ă���m�[�h�̔ԍ�
		Node& node = nodes[nodeIndex];
		
		if (strcmp(node.name, name) == 0)
		{
			return &node;
		}
	}
	//������Ȃ�����
	return nullptr;
}