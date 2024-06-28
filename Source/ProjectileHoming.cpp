#include "ProjectileHoming.h"

//コンストラクタ
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
	:Projectile(manager)
{
	model = new Model("Data/Model/Sword/Sword.mdl");

	//モデルのスケーリング
	scale.x = scale.y = scale.z = 3.0f;
}

//デストラクタ
ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

//更新処理
void ProjectileHoming::Update(float elapsedTime)
{
	//寿命処理
	lifeTimer -= elapsedTime;
	if (lifeTimer <= 0.0f)
	{
		//自分を削除
		Destroy();
	}
	//移動
	{
		float moveSpeed = this->moveSpeed * elapsedTime;
		position.x += direction.x * moveSpeed;
		position.y += direction.y * moveSpeed;
		position.z += direction.z * moveSpeed;
	}

	//旋回
	{
		float turnSpeed = this->turnSpeed * elapsedTime;
	}
		//ターゲットまでのベクトルを算出
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

	//ゼロベクトルでないなら回転処理
	DirectX::XMVECTOR LenghSp = DirectX::XMVector3LengthSq(Vec);
	float lenghSq;
	DirectX::XMStoreFloat(&lenghSq, LenghSp);
	if (lenghSq > 0.00001f)
	{
		//ターゲットまでのベクトルを単位ベクトル化(正規化)
		Vec = DirectX::XMVector3Normalize(Vec);
		//向いている方向(前方向ベクトル)を算出
		DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);;
		//前方向ベクトルとターゲットまでの内積を求める
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction ,Vec);

		float dot;
		DirectX::XMStoreFloat(&dot, Dot);
		//2つのベクトルの角度が小さいほど
		//1.0に近づく性質を利用して回転速度計算
		float rot = 1.0f - dot;
		if (rot > turnSpeed)
		{
			rot = turnSpeed;
		}

		//回転角度(rot)があるなら回転処理
		//回転角度は絶対値で、0より上かどうかで計算
		if (rot > 0.0001f)
		{
			//回転軸を計算
			DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
			Axis = DirectX::XMVector3Normalize(Axis);

			//回転軸と回転量から回転行列を算出
			DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

			//現在の行列を回転させる
			//回転後の姿勢=現在の姿勢*回転行列
			DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
			Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

			//回転の前方向を取り出し
			Direction = DirectX::XMVector3Normalize(Transform.r[2]);
			DirectX::XMStoreFloat3(&direction, Direction);
			//単位ベクトル化する
			DirectX::XMVector3Normalize(Direction);
		}
	}
	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform(transform);
}

//描画処理
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//発射
void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction,
							  const DirectX::XMFLOAT3& position,
							  const DirectX::XMFLOAT3& target)
{
	this->direction = direction;
	this->position = position;
	this->target = target;

	UpdateTransform();
}