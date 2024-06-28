#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

static Player* instance = nullptr;

//インスタンス取得
Player& Player::Instance()
{
	return *instance;
}

//コンストラクタ
Player::Player()
{
	//インスタンスポインタ設定
	instance = this;

	model = new Model("Data/Model/Jammo/Jammo.mdl");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");

	//待機ステートへ遷移
	TransitionIdleState();
}

//デストラクタ
Player::~Player()
{
	delete model;

	delete hitEffect;
}

//更新処理
void Player::Update(float elapsedTime)
{
	if (position.y < -10.0f)position = { 0, 1, 0 };
	
	//ステート毎の処理
	switch (state)
	{
	case State::Idle:
		UpdateIdleState(elapsedTime);
		break;
	case State::Move:
		UpdateMoveState(elapsedTime);
		break;
	case State::Jump:
		UpdateJumpState(elapsedTime);
		break;
	case State::Land:
		UpdateLandState(elapsedTime);
		break;
	case State::JumpFlip:
		UpdateJumpFlipState(elapsedTime);
		break;
	case State::Attack:
		UpdateAttackState(elapsedTime);
		break;
	case State::Damage:
		UpdateDamageState(elapsedTime);
		break;
	case State::Death:
		UpdateDeathState(elapsedTime);
		break;
	case State::Revive:
		UpdateReviveState(elapsedTime);
		break;
	}

	//速力更新処理
	UpdateVelocity(elapsedTime);

	//プレイヤーと敵の衝突判定
	CollisionPlayerVsEnemies();

	//弾丸と敵の衝突判定
	CollisionProjectileVsEnemies();

	//弾丸更新処理
	projectileManager.Update(elapsedTime);

	//無敵時間更新処理
	UpdateInvincibleTimer(elapsedTime);

	//オブジェクト行列を更新
	UpdateTransform();

	//モデルアニメーション更新処理
	model->UpdateAnimation(elapsedTime);

	//モデル行列更新
	model->UpdateTransform(transform);
}

//描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);

	//弾丸描画処理
	projectileManager.Render(dc, shader);
}
//デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiTreeNodeFlags_None))
	{
		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//位置
			ImGui::InputFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
		}
	}
	ImGui::End();
}

//デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//衝突判定用にデバッグ球を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	//弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();

	//攻撃衝突用の左手ノードのデバッグ球を描画
	if (attackColisionFlag)
	{
		Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
		debugRenderer->DrawSphere(DirectX::XMFLOAT3(
			leftHandBone->worldTransform._41,
			leftHandBone->worldTransform._42,
			leftHandBone->worldTransform._43),
			leftHandRadius,
			DirectX::XMFLOAT4(1, 0, 0, 1)
		);
	}
}

//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	//カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	//移動ベクトルはXZ方面に水平なベクトルになるようにする

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLengh = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLengh > 0.0f)
	{
		//単位ベクトル化
		cameraRightX /= cameraRightLengh;
		cameraRightZ /= cameraRightLengh;
	}

	//カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLengh = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLengh > 0.0f)
	{
		//単位ベクトル化
		cameraFrontX /= cameraFrontLengh;
		cameraFrontZ /= cameraFrontLengh;
	}
	//スティックの水平入力値をカメラ右側に反映し、
	//スティックの水平入力値をカメラ前側に反映し、
	//進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	//Y軸方向には発動しない
	vec.y = 0.0f;

	return vec;
}

//移動入力処理
bool Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	//移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	//進行ベクトルが0でない場合入力された
	if (moveVec.x != 0.0f || moveVec.z != 0.0f)
	{
		return true;
	}
	else return false;
}

//プレイヤーとエネミーとの衝突判定
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
		{
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);
			//上から踏んだ場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				//小ジャンプする
				Jump(jumpSpeed * 0.5f);
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				//押し出し後の位置設定
				enemy->SetPosition(outPosition);
			}
		}
	}
}

//ジャンプ入力処理
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{		
		//ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			//ジャンプ
			jumpCount++;
			Jump(jumpSpeed);
			//ジャンプ入力した
			return true;
		}		
	}
	return false;
}

//着地した際に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;

	if (velocity.y < -10.0f)
	{
		if (state != State::Damage && state != State::Death)
		{
			TransitionLandState();
		}
	}
}

//ダメージを受けた際に呼ばれる
void Player::OnDamaged()
{
	//ダメージステートへ遷移
	TransitionDamageState();
}

//死亡した際に呼ばれる
void Player::OnDead()
{
	//死亡ステートへ遷移
	TransitionDeathState();
}

void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//直進弾丸
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = transform._31;
		dir.y = transform._32;
		dir.z= transform._33;
		//発射位置
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y;
		pos.z = position.z;

		ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}
	//追尾弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y); //dir.x = 
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;

		//ターゲット(デフォルトはプレイヤーの前方)
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		//一番近くの敵をターゲット
		//一番遠い場所を設定
		float dist = FLT_MAX;
		//敵のマネージャーを取得
		EnemyManager& enemyManager = EnemyManager::Instance();
		//敵の数を取得
		int enemyCount = enemyManager.GetEnemyCount();

		//一番遠い敵を敵の数だけチェック
		for (int i = 0; i < enemyCount; i++)
		{
			//敵との距離判定
			//i番目の敵を取得
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			//プレイヤーの位置をXMVECTORに変換
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);			
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			// 敵方向へのベクトルを計算
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			//その長さを計算し
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			//単一の値にする
			float d;
			DirectX::XMStoreFloat(&d, D);
			//今まで計算した距離の中で一番小さい距離なら
			if (d < dist)
			{
				dist = d;
				//targetの位置を更新(敵の位置を設定)
				target = enemy->GetPosition();
				//targetのyだけ腰の高さにしておく
				target.y += enemy->GetHeight() * 0.5f;
			}
		}
		//発射
		ProjectileHoming* projectilehoming = new ProjectileHoming(&projectileManager);
		projectilehoming->Launch(dir, pos, target);
	}
}

//弾丸と敵の衝突判定
void Player::CollisionProjectileVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸とすべての敵を総当たりで衝突判定
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; i++)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; j++)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSpherVsCyling
			(projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				//ダメージを与える
				if(enemy->ApplyDamage(1, 0.5f))
				{
					//吹き飛ばす
					{
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;//吹き飛ばす力

						//敵の位置を取得
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						//弾の位置を取得
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
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
						
						enemy->AddImpulse(impulse);
					}
					//ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}
					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}

//攻撃入力処理
bool Player::InputAttack()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}
	return false;
}

//待機ステートへ遷移
void Player::TransitionIdleState()
{
	state = State::Idle;

	//待機アニメーション再生
	model->PlayAnimation(Anim_Idle, true);
}

//待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime)
{
	//移動入力処理
	//bool Move = InputMove(elapsedTime);
	if (InputMove(elapsedTime))
	{
		TransitionMoveState();
	}

	//ジャンプ入力処理
	if (InputJump())
	{
		TransitionJumpState();
	}

	//攻撃入力処理
	if (InputAttack())
	{
		TransitionAttackState();
	}

	//弾丸入力処理
	InputProjectile();
}

void Player::TransitionMoveState()
{
	state = State::Move;

	//走りアニメーション再生
	model->PlayAnimation(Anim_Running, true);
}

//移動ステート更新処理
void Player::UpdateMoveState(float elapsedTime)
{
	//bool Move = InputMove(elapsedTime);
	if (!InputMove(elapsedTime))
	{
		TransitionIdleState();	
	}

	//ジャンプ入力処理
	if (InputJump())
	{
		TransitionJumpState();
	}

	//攻撃入力処理
	if (InputAttack())
	{
		TransitionAttackState();
	}

	//弾丸入力処理
	InputProjectile();
}

//ジャンプステートへ遷移
void Player::TransitionJumpState()
{
	state = State::Jump;

	//ジャンプアニメーション再生
	model->PlayAnimation(Anim_Jump, false);
}

//ジャンプステート更新処理
void Player::UpdateJumpState(float elapsedTime)
{
	InputMove(elapsedTime);

	//ジャンプ入力処理
	if (InputJump())
	{
		TransitionJumpFlipState();
	}
}

//着地ステートへ遷移
void Player::TransitionLandState()
{
	state = State::Land;

	//ジャンプアニメーション再生
	model->PlayAnimation(Anim_Landing, false);
}

//着地ステート更新処理
void Player::UpdateLandState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

//2段ジャンプステートへ遷移
void Player::TransitionJumpFlipState()
{
	state = State::JumpFlip;

	//ジャンプアニメーション再生
	model->PlayAnimation(Anim_Jump_Flip, false);
}

//2段ジャンプステート更新処理
void Player::UpdateJumpFlipState(float elapsedTime)
{
	InputMove(elapsedTime);
}

//攻撃ステートへ遷移
void Player::TransitionAttackState()
{
	state = State::Attack;

	//攻撃アニメーション再生
	model->PlayAnimation(Anim_Attack, false);
}

//攻撃ステート更新処理
void Player::UpdateAttackState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
	//任意のアニメーション再生区間でのみ当たり判定を発生させる
	float animetionTime = model->GetCurrentAnimetionSeconds();
	if (animetionTime > 0.3f && animetionTime < 0.4f)
	{
		attackColisionFlag = true;
	}
	else attackColisionFlag = false;

	if (attackColisionFlag)
	{
		//左手ノードとエネミーの衝突処理
		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
	}
}

//ノードと敵の衝突処理
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
	//ノード取得
	Model::Node* node = model->FindNode(nodeName);
	//ノード位置取得
	DirectX::XMFLOAT3 nodePosition;
	nodePosition.x = node->worldTransform._41;
	nodePosition.y = node->worldTransform._42;
	nodePosition.z = node->worldTransform._43;

	EnemyManager& enemyManager = EnemyManager::Instance();

	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		//衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSpherVsCyling(
			nodePosition,
			nodeRadius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition
		))
			//ダメージを与える
			if (enemy->ApplyDamage(1, 0.5f))
			{
				//吹き飛ばす
				{
					DirectX::XMFLOAT3 impulse;
					const float power = 10.0f;//吹き飛ばす力

					//敵の位置を取得
					const DirectX::XMFLOAT3& e = enemy->GetPosition();
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

					enemy->AddImpulse(impulse);
				}
				//ヒットエフェクト再生
				{
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					hitEffect->Play(e);
				}
			}
	}
}

//ダメージステートへ遷移
void Player::TransitionDamageState()
{
	state = State::Damage;

	//ダメージアニメーション再生
	model->PlayAnimation(Anim_GetHit1, false);
}

//ダメージステート更新処理
void Player::UpdateDamageState(float elapsedTime)
{
	//ダメージアニメーション終了後に待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}

//死亡ステートへ遷移
void Player::TransitionDeathState()
{
	state = State::Death;

	//ダメージアニメーション再生
	model->PlayAnimation(Anim_Death, false);
}

//死亡ステート更新処理
void Player::UpdateDeathState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		//ボタンを押したら復活
		GamePad& gamePad = Input::Instance().GetGamePad();
		if (gamePad.GetButtonDown() & GamePad::BTN_A)
		{
			TransitionReviveState();
		}
	}
}

//復活ステートへ遷移
void Player::TransitionReviveState()
{
	state = State::Revive;

	//復活アニメーション再生
	model->PlayAnimation(Anim_Revive, false);
}

//復活ステート更新処理
void Player::UpdateReviveState(float elapsedTime)
{
	//復活アニメーション終了後に待機ステートへ遷移
	if (!model->IsPlayAnimation())
	{
		health = 5;
		invincibleTimer = 1.0f;
		TransitionIdleState();
	}
}