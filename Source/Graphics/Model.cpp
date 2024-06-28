#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "ResourceManager.h"

// コンストラクタ
Model::Model(const char* filename)
{
	// リソース読み込み
	//resource = std::make_shared<ModelResource>();
	//resource->Load(Graphics::Instance().GetDevice(), filename);
	resource = ResourceManager::Instance().LoadModelResource(filename);

	// ノード
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

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
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

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}
}

//アニメーション更新処理
void Model::UpdateAnimation(float elapsedTime)
{
	//再生中なら抜ける
	if (!IsPlayAnimation())return;

	//ブレンド率の計算
	float blendRate = 1.0f;
	if (animationBlendSeconds > 0.0f)
	{
		animationBlendTime = currentAnimationSeconds;
		blendRate = animationBlendTime / animationBlendSeconds;		
	}

	//指定のアニメーションデータを取得
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	//アニメーションデータからキーフレームデータリストを取得
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		//現在の時間がどのキーフレームの間にいるか計算
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentAnimationSeconds >= keyframe0.seconds &&
			currentAnimationSeconds < keyframe1.seconds)
		{
			//再生時間とキーフレームの時間から補完率を算出
			float rate = (currentAnimationSeconds - keyframe0.seconds)
							/ (keyframe1.seconds);

			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				//2つのキーフレーム間の保管処理
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				//ブレンド保管処理
				if (blendRate < 1.0f)
				{
					//現在の姿勢と次のキーフレームとの姿勢の保管
					DirectX::XMVECTOR current_Position = DirectX::XMLoadFloat3(&node.translate);
					DirectX::XMVECTOR current_Rotation = DirectX::XMLoadFloat4(&node.rotate);
					DirectX::XMVECTOR key1_Position = DirectX::XMLoadFloat3(&key1.translate);
					DirectX::XMVECTOR key1_Rotation = DirectX::XMLoadFloat4(&key1.rotate);

					DirectX::XMVECTOR Position = DirectX::XMVectorLerp(current_Position, key1_Position, blendRate);
					DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(current_Rotation, key1_Rotation, blendRate);
					DirectX::XMStoreFloat3(&node.translate, Position);
					DirectX::XMStoreFloat4(&node.rotate, Rotation);
				}
				//通常の計算
				else
				{
					//前のキーフレームと次のキーフレームの姿勢を補完
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
	//最終フレーム処理
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}

	//経過時間
	currentAnimationSeconds += elapsedTime;

	//再生時間が終端時間を超えたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		
		if (animationLoopFlag == true)//ループがtrueなら
		{
			//再生時間を巻き戻す
			currentAnimationSeconds -= animation.secondsLength;
			animationEndFlag = false;
		}

		else
		{
			animationEndFlag = true;
		}
	}
}

//アニメーション再生
void Model::PlayAnimation(int index, bool loop, float blendSecons)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0f;
	animationBlendSeconds = blendSecons;
}

//アニメーション再生中か
bool Model::IsPlayAnimation()const
{
	if (currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= resource->GetAnimations().size())return false;
	return true;
}

//ノード検索
Model::Node* Model::FindNode(const char* name)
{
	//全てのノードと総当たりで名前比較
	int nodeCount = static_cast<int>(nodes.size());
	for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		//現在見ているノードの番号
		Node& node = nodes[nodeIndex];
		
		if (strcmp(node.name, name) == 0)
		{
			return &node;
		}
	}
	//見つからなかった
	return nullptr;
}