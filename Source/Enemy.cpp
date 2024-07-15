#include "Enemy.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"
#include "Graphics/Graphics.h"

static Enemy* instance = nullptr;

//インスタンス取得
Enemy& Enemy::Instance()
{
	return *instance;
}

//コンストラクタ
Enemy::Enemy()
{
	//インスタンスポインタ設定
	instance = this;

	model = new Model("Data/Model/Slime/Slime.mdl");

	JumpStart = Audio::Instance().LoadAudioSource("Data/SE/JumpStart.wav");
	JumpEnd = Audio::Instance().LoadAudioSource("Data/SE/JumpEnd.wav");
	Hit = Audio::Instance().LoadAudioSource("Data/SE/Hit.wav");

	//モデルのスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	//徘徊ステートへ遷移
	TransitionWanderState();
}

//デストラクタ
Enemy::~Enemy()
{
	JumpStart->Stop();
	JumpEnd->Stop();
	Hit->Stop();

	delete model;
}

//更新処理
void Enemy::Update(float elapsedTime)
{
	if (position.y < -10.0f)
	{
		position = { 0, 1, -10 };
		SetRandomTargetPosition();
	}
	//ステート毎の更新処理
	switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	/*case State::Pursuit:
		UpdatePursuitState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::IdleBattle:
		UpdateIdleBattleState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;*/
	}

	//速力更新処理
	UpdateVelocity(elapsedTime);

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	CollisionPlayerVsEnemies();

	SetRandomTargetPosition();

	//モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	//モデル行列更新
	model->UpdateTransform(transform);
}

//描画処理
void Enemy::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

////ダメージを受けた際に呼ばれる
//void Enemy::OnDamaged()
//{
//	//ダメージステートへ遷移
//	TransitionDamageState();
//}
//
////死亡したときに呼ばれる
//void Enemy::OnDead()
//{
//	//死亡ステートへ遷移
//	TransitionDeathState();
//}

//デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	////縄張り範囲をデバッグ円柱描画
	debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
		DirectX::XMFLOAT4(0, 1, 0, 1));

	//ターゲット位置をデバッグ球描画
	debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	//索敵範囲をデバッグ円柱描画
	debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	//攻撃範囲をデバッグ円柱描画
	debugRenderer->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void Enemy::eJump()
{	
		//ジャンプ回数制限
	if (jumpCount < jumpLimit && jumpCT <= 0.0f)
	{
		JumpStart->Play(false);//ジャンプSE再生
		jumpFlag = true;
		//ジャンプ
		 jumpCount++;
		 Jump(jumpSpeed);
		 //敵の位置がプレイヤーと比べて大きいかどうかで力の向きを変える
		 DirectX::XMFLOAT3 Ppos = Player::Instance().GetPosition();
		 if (position.x - Ppos.x > 0.0f)
		 {
			 power = 1.0f;
		 }
		 else if (position.x - Ppos.x < 0.0f)
		 {
			 power = -1.0f;
		 }
	}
}

//着地した際に呼ばれる
void Enemy::OnLanding()
{
	if (jumpFlag == true)
	{
		JumpEnd->Play(false);//着地SE再生
		jumpCT = 2.0f;
		jumpFlag = false;
		jumpCount = 0;
		if (moveSpeed >= 1.5f)
		{
			moveSpeed /= 2.0f;
		}
	}
}

//プレイヤーとエネミーとの衝突判定
void Enemy::CollisionPlayerVsEnemies()
{
	Player& player = Player::Instance();
	//衝突処理
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition
	))
	{
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&player.GetPosition());
		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
		DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
		DirectX::XMFLOAT3 normal;
		DirectX::XMStoreFloat3(&normal, N);
		{
			float L{};
			DirectX::XMFLOAT3 l{};
			DirectX::XMStoreFloat3(&l, DirectX::XMVectorScale(V, -1.0f));
			if (l.x > 0.0f)
			{
				L = 1.0f;
			}
			else if (l.x < 0.0f)
			{
				L = -1.0f;
			}

			if (jumpFlag && !player.GetJump())
			{
				Hit->Play(false);//攻撃のヒット音再生
				outPosition.x = position.x + (2.0f * L);
				player.SetPosition(outPosition);
			}
			else if (power != 0.0f && player.GetPower() * -1.0f == power)
			{
				outPosition.x = position.x + ((player.GetRadius() * 0.5f) * L);
				player.SetPosition(outPosition);
			}
			else if (power != 0.0f && player.GetPower() == 0.0f)
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				player.SetPosition(outPosition);
			}
			else
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				player.SetPosition(outPosition);
			}
		}
	}
}

//縄張り設定
void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

//ターゲット位置をランダムに設定
void Enemy::SetRandomTargetPosition()
{
	if ((position.x <= targetPosition.x + 0.5f && position.x >= targetPosition.x - 0.5f)
		|| (position.z <= targetPosition.z + 0.5f && position.z >= targetPosition.z - 0.5f))
	{
		targetRandomX = static_cast<int>(Mathf::RandomRange(-2.5f, 2.5f));
		
		if (targetRandomX >= 2 || targetRandomX <= -2)
		{
			eJump();
		}
		targetPosition.x = position.x + static_cast<float>(targetRandomX);
		targetPosition.y = 0.5f;
		targetPosition.z = position.z + 3.0f;
	}
}

//目標地点へ移動
void Enemy::MoveToTarget(float elapsedTime, float speedRate)
{
	moveSpeed += Velocity * elapsedTime;
	if (moveSpeed >= maxSpeed)
	{
		moveSpeed = maxSpeed;
	}
	if (jumpCT > 0.0f)
	{
		jumpCT -= 1.0f * elapsedTime;
	}
	//ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	if (vx != 0.0f && moveSpeed >= maxSpeed)
	{
		moveSpeed = 2.3f;
	}

	if (vx > 0.0f)
	{
		power = 1.0f;
	}
	else if (vx < 0.0f)
	{
		power = -1.0f;
	}
	else if (!jumpFlag && power != 0.0f)
	{
		//非ジャンプ時なら力を0に戻す	
		power = 0.0f;
	}

	//移動処理
	if (jumpFlag)
	{
		Move(vx * 0.5f, vz, moveSpeed);
	}
	else Move(vx, vz, moveSpeed);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool Enemy::SearchPlayer()
{
	//プレイヤーとの高低差を考慮して3Dでの距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	targetRandomX = 5;
	if (dist < searchRange)
	{
		float distXZ = sqrtf(vx * vx + vz * vz);
		//単位ベクトル化
		vx /= distXZ;
		vz /= distXZ;
		//前方ベクトル
		float frontX = sinf(angle.y);
		float frontZ = cosf(angle.y);
		//2つのベクトルの内積値で前後判定
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

//ノードとプレイヤーの衝突処理
void Enemy::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	//ノード取得
	Model::Node* node = model->FindNode(nodeName);
	//ノード位置取得
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	Graphics::Instance().GetDebugRenderer()->DrawSphere(
		nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
	);

	Player& player = Player::Instance();

	//衝突処理
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectSpherVsCyling(
		nodePosition,
		nodeRadius,
		player.GetPosition(),
		player.GetRadius(),
		player.GetHeight(),
		outPosition))
	{
		//ダメージを与える
		if (player.ApplyDamage(1, 0.5f))
		{
			//吹き飛ばす
			{
				DirectX::XMFLOAT3 impulse;
				const float power = 10.0f;//吹き飛ばす力

				//敵の位置を取得
				const DirectX::XMFLOAT3& e = player.GetPosition();
				//弾の位置を取得
				const DirectX::XMFLOAT3& p = nodePosition;
				//弾から敵へのベクトルを取得
				float vx = e.x - p.x;
				float vz = e.z - p.z;
				//そのベクトルを正規化(長さを計算し、長さで割る)
				float lengthXZ = sqrtf(vx * vx + vz * vz);
				vx /= lengthXZ;
				vz /= lengthXZ;
				//衝撃の値を設定(xzは正規化したベクトルをpower文スケーリング)
				impulse.x = vx * power;
				impulse.y = power * 0.5f;
				impulse.z = vz * power;

				player.AddImpulse(impulse);
			}
		}
	}
}


//徘徊ステートへ遷移
void Enemy::TransitionWanderState()
{
	state = State::Wander;

	//目標地点設定
	//SetRandomTargetPosition();
}

//徘徊ステート更新処理
void Enemy::UpdateWanderState(float elapsedTime)
{
	//目標地点までXZ平面での距離判定
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;
	if (distSq < radius * radius)
	{
		//待機ステートへ遷移
		TransitionIdleState();
	}
	//目標地点へ移動
	MoveToTarget(elapsedTime, 0.5f);
	SetTerritory(position, 10.0f);

	////プレイヤー索敵
	//if (SearchPlayer())
	//{
	//	//見つかったら追跡ステートへ遷移
	//	TransitionPursuitState();
	//}
}

//待機ステートへ遷移
void Enemy::TransitionIdleState()
{
	state = State::Idle;

	//タイマーをランダム設定
	stateTimer = static_cast<float>(Mathf::RandomRange(3, 5));
}

//待機ステート更新処理
void Enemy::UpdateIdleState(float elapsedTime)
{
	//タイマー処理
	stateTimer -= elapsedTime;

	//徘徊ステートへ遷移
	TransitionWanderState();
}

////追跡ステートへ遷移
//void Enemy::TransitionPursuitState()
//{
//	state = State::Pursuit;
//
//	//数秒間追跡するタイマーをランダム設定
//	stateTimer = static_cast<float>(Mathf::RandomRange(3, 5));
//}
//
////追跡ステート更新処理
//void Enemy::UpdatePursuitState(float elapsedTime)
//{
//	//目標地点へ移動
//	MoveToTarget(elapsedTime, 1.0f);
//	SetTerritory(position, 10.0f);
//
//	//タイマー処理
//	if (!SearchPlayer())
//	{
//		stateTimer -= elapsedTime;
//	}
//
//	if (stateTimer < 0.0f)
//	{
//		//待機ステートへ遷移
//		TransitionIdleState();
//	}
//
//	float vx = targetPosition.x - position.x;
//	float vy = targetPosition.y - position.y;
//	float vz = targetPosition.z - position.z;
//	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//	if (dist < attackRange)
//	{
//		//攻撃ステートへ遷移
//		TransitionAttackState();
//	}
//}
//
////攻撃ステートへ遷移
//void Enemy::TransitionAttackState()
//{
//	state = State::Attack;
//}
//
////攻撃ステート更新処理
//void Enemy::UpdateAttackState(float elapsedTime)
//{
//	//任意のアニメーション区間でのみ衝突判定処理をする
//	float animationTime = model->GetCurrentAnimetionSeconds();
//	if (animationTime >= 0.1f && animationTime <= 0.35f)
//	{
//		//目玉のノードとプレイヤーの衝突判定
//		CollisionNodeVsPlayer("EyeBall", 0.2f);
//	}
//	//攻撃アニメーションが終了したら戦闘待機
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleBattleState();
//	}
//}
//
////戦闘待機ステートへ遷移
//void Enemy::TransitionIdleBattleState()
//{
//	state = State::IdleBattle;
//
//	//数秒間待機するタイマーをランダムで設定
//	stateTimer = 0.0f;
//}
//
////戦闘待機ステート更新処理
//void Enemy::UpdateIdleBattleState(float elapsedTime)
//{
//	DirectX::XMFLOAT3 PlayerPosition = Player::Instance().GetPosition();
//	//タイマー処理
//	stateTimer -= elapsedTime;
//	if (stateTimer < 0.0f)
//	{
//		//プレイヤーが攻撃範囲にいれば攻撃ステートへ遷移
//		float vx = PlayerPosition.x - position.x;
//		float vy = PlayerPosition.y - position.y;
//		float vz = PlayerPosition.z - position.z;
//		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
//		if (dist < attackRange)
//		{
//			//攻撃ステートへ遷移
//			TransitionAttackState();
//		}
//		else
//		{
//			//徘徊ステートへ遷移
//			TransitionWanderState();
//		}
//	}
//	MoveToTarget(elapsedTime, 0.0f);
//}
//
////ダメージステートへ遷移
//void Enemy::TransitionDamageState()
//{
//	state = State::Damage;
//}
//
////ダメージステート更新処理
//void Enemy::UpdateDamageState(float elapsedTime)
//{
//	//ダメージアニメーションが終わったら戦闘待機ステートへ遷移
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleBattleState();
//	}
//}
//
////死亡ステートへ遷移
//void Enemy::TransitionDeathState()
//{
//	state = State::Death;
//}
//
////死亡ステート更新処理
//void Enemy::UpdateDeathState(float elapsedTime)
//{
//	//アニメーションが終わったら自信を破棄
//	if (!model->IsPlayAnimation())
//	{
//		
//	}
//}