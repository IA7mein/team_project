#include "EnemySlime.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

//コンストラクタ
EnemySlime::EnemySlime()
{
	model = new Model("Data/Model/Slime/Slime.mdl");

	//モデルのスケーリング
	scale.x = scale.y = scale.z = 0.05f;

	//幅、高さ設定
	radius = 5.0f;
	height = 1.5f;

	//徘徊ステートへ遷移
	TransitionWanderState();
}

//デストラクタ
EnemySlime::~EnemySlime()
{
	delete model;
}

//更新処理
void EnemySlime::Update(float elapsedTime)
{
	//ステート毎の更新処理
	/*switch (state)
	{
	case State::Wander:
		UpdateWanderState(elapsedTime);
		break;
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	case State::Pursuit:
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
		break;
	}*/

	//速力更新処理
	UpdateVelocity(elapsedTime);

	//無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	//モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	//モデル行列更新
	model->UpdateTransform(transform);

	position.y = 5.4f;
}

//描画処理
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//ダメージを受けた際に呼ばれる
void EnemySlime::OnDamaged()
{
	//ダメージステートへ遷移
	TransitionDamageState();
}

//死亡したときに呼ばれる
void EnemySlime::OnDead()
{
	//死亡ステートへ遷移
	TransitionDeathState();
}

//デバッグプリミティブ描画
void EnemySlime::DrawDebugPrimitive()
{
	//基底クラスのデバッグプリミティブ描画
	Enemy::DrawDebugPrimitive();

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//////縄張り範囲をデバッグ円柱描画
	//debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f,
	//	DirectX::XMFLOAT4(0, 1, 0, 1));

	////ターゲット位置をデバッグ球描画
	//debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));

	////索敵範囲をデバッグ円柱描画
	//debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

	////攻撃範囲をデバッグ円柱描画
	//debugRenderer->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

//縄張り設定
void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
       
//ターゲット位置をランzダムに設定
void EnemySlime::SetRandomTargetPosition()
{
	float targetRandomX{};//X軸ランダム値
	float targetRandomZ{};//Z軸ランダム値
	if (position.x != targetPosition.x
		&& position.y != targetPosition.y
		&& position.z != targetPosition.z)
	{
		targetRandomX = Mathf::RandomRange(-5.0f, 5.0f);
		targetRandomZ = Mathf::RandomRange(-5.0f, 5.0f);
	}
	targetPosition.x = position.x + targetRandomX;
	targetPosition.y = 0.5f;
	targetPosition.z = position.z + targetRandomZ;
}

//目標地点へ移動
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
	//ターゲット方向への進行ベクトルを算出
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	//移動処理
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool EnemySlime::SearchPlayer()
{
	//プレイヤーとの高低差を考慮して3Dでの距離判定をする
	const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
	float vx = playerPosition.x - position.x;
	float vy = playerPosition.y - position.y;
	float vz = playerPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
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
void EnemySlime::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
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
void EnemySlime::TransitionWanderState()
{
	state = State::Wander;

	//目標地点設定
	SetRandomTargetPosition();

	//歩きアニメーション再生
	model->PlayAnimation(Anim_WalkFWD, true);
}

//徘徊ステート更新処理
void EnemySlime::UpdateWanderState(float elapsedTime)
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

	//プレイヤー索敵
	if (SearchPlayer())
	{
		//見つかったら追跡ステートへ遷移
		TransitionPursuitState();
	}
}

//待機ステートへ遷移
void EnemySlime::TransitionIdleState()
{
	state = State::Idle;

	//タイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	//待機アニメーション再生
	model->PlayAnimation(Anim_IdleNormal, true);
}

//待機ステート更新処理
void EnemySlime::UpdateIdleState(float elapsedTime)
{
	//タイマー処理
	stateTimer -= elapsedTime;

	if (stateTimer < 0.0f)
	{
		//徘徊ステートへ遷移
		TransitionWanderState();
	}
}

//追跡ステートへ遷移
void EnemySlime::TransitionPursuitState()
{
	state = State::Pursuit;

	//数秒間追跡するタイマーをランダム設定
	stateTimer = Mathf::RandomRange(3.0f, 5.0f);

	//歩きアニメーション再生
	model->PlayAnimation(Anim_RunFWD, true);
}

//追跡ステート更新処理
void EnemySlime::UpdatePursuitState(float elapsedTime)
{
	//目標位置をプレイヤーの位置で上書き
	targetPosition = Player::Instance().GetPosition();

	//目標地点へ移動
	MoveToTarget(elapsedTime, 1.0f);
	SetTerritory(position, 10.0f);

	//タイマー処理
	if (!SearchPlayer())
	{
		stateTimer -= elapsedTime;
	}

	if (stateTimer < 0.0f)
	{
		//待機ステートへ遷移
		TransitionIdleState();
	}

	float vx = targetPosition.x - position.x;
	float vy = targetPosition.y - position.y;
	float vz = targetPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);
	if (dist < attackRange)
	{
		//攻撃ステートへ遷移
		TransitionAttackState();
	}
}

//攻撃ステートへ遷移
void EnemySlime::TransitionAttackState()
{
	state = State::Attack;

	//攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack1, false);
}

//攻撃ステート更新処理
void EnemySlime::UpdateAttackState(float elapsedTime)
{
	//任意のアニメーション区間でのみ衝突判定処理をする
	float animationTime = model->GetCurrentAnimetionSeconds();
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		//目玉のノードとプレイヤーの衝突判定
		CollisionNodeVsPlayer("EyeBall", 0.2f);
	}
	//攻撃アニメーションが終了したら戦闘待機
	if (!model->IsPlayAnimation())
	{
		TransitionIdleBattleState();
	}
}

//戦闘待機ステートへ遷移
void EnemySlime::TransitionIdleBattleState()
{
	state = State::IdleBattle;

	//数秒間待機するタイマーをランダムで設定
	stateTimer = Mathf::RandomRange(2.0f, 3.0f);

	//戦闘待機アニメーション再生
	model->PlayAnimation(Anim_IdleBattle, true);
}

//戦闘待機ステート更新処理
void EnemySlime::UpdateIdleBattleState(float elapsedTime)
{
	//目標地点をプレイヤーに設定
	targetPosition = Player::Instance().GetPosition();

	//タイマー処理
	stateTimer -= elapsedTime;
	if (stateTimer < 0.0f)
	{
		//プレイヤーが攻撃範囲にいれば攻撃ステートへ遷移
		float vx = targetPosition.x - position.x;
		float vy = targetPosition.y - position.y;
		float vz = targetPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);
		if (dist < attackRange)
		{
			//攻撃ステートへ遷移
			TransitionAttackState();
		}
		else
		{
			//徘徊ステートへ遷移
			TransitionWanderState();
		}
	}
	MoveToTarget(elapsedTime, 0.0f);
}

//ダメージステートへ遷移
void EnemySlime::TransitionDamageState()
{
	state = State::Damage;

	//ダメージアニメーション再生
	model->PlayAnimation(Anim_GetHit, false);
}

//ダメージステート更新処理
void EnemySlime::UpdateDamageState(float elapsedTime)
{
	//ダメージアニメーションが終わったら戦闘待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionIdleBattleState();
	}
}

//死亡ステートへ遷移
void EnemySlime::TransitionDeathState()
{
	state = State::Death;

	//死亡アニメーション再生
	model->PlayAnimation(Anim_Die, false);
}

//死亡ステート更新処理
void EnemySlime::UpdateDeathState(float elapsedTime)
{
	//アニメーションが終わったら自信を破棄
	if (!model->IsPlayAnimation())
	{
		Destoroy();
	}
}