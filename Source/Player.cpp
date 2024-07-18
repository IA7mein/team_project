#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "Enemy.h"
#include "Collision.h"
#include "SceneTitle.h"
static Player* instance = nullptr;
bool goal;
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

	model = new Model("Data/Model/Player/1P.mdl");

	JumpStart = Audio::Instance().LoadAudioSource("Data/SE/JumpStart.wav");
	JumpEnd = Audio::Instance().LoadAudioSource("Data/SE/JumpEnd.wav");
	Hit = Audio::Instance().LoadAudioSource("Data/SE/Hit.wav");
	ItemGet = Audio::Instance().LoadAudioSource("Data/SE/ItemGet.wav");
	HitObject = Audio::Instance().LoadAudioSource("Data/SE/HitObject.wav");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.1f;

	position.y = 5.4f;

	//ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/damage.efk");
	hart = new Effect("Data/Effect/efec1.efk");
	shield = new Effect("Data/Effect/efec3.efk");
	goal = false;
	//待機ステートへ遷移
	TransitionIdleState();
}

//デストラクタ
Player::~Player()
{
	JumpStart->Stop();
	JumpEnd->Stop();
	Hit->Stop();
	ItemGet->Stop();
	HitObject->Stop();

	delete model;

	delete hitEffect;
	delete hart;
	delete shield;
}

//更新処理
void Player::Update(float elapsedTime)
{
	if (position.y < -10.0f)position = { 0, 1, -10 };

	//if(ゴールについたら)
	{
		goal = true;
	}

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
	/*case State::JumpFlip:
		UpdateJumpFlipState(elapsedTime);
		break;*/
	}
	
	//速力更新処理
	UpdateVelocity(elapsedTime);

	//プレイヤーと敵の衝突判定
	if(muluchmode==false)CollisionPlayerVsEnemies();

	//プレイヤーとプレイヤーの衝突判定
	if (muluchmode)//ここに;

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
			ImGui::InputFloat("Power", &power);

			/*float enemypower = Enemy::Instance().GetPower();
			ImGui::InputFloat("EnemyPower", &enemypower);
			ImGui::Checkbox("jumpFlag", &jumpFlag);
			bool enemyjump = Enemy::Instance().GetJump();
			ImGui::Checkbox("EnemyjumpFlag", &enemyjump);*/
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
	//進行ベクトルを計算する
	if (ax < 0.0f)
	{
		ax = -1.0f;
	}
	else if (ax > 0.0f)
	{
		ax = 1.0f;
	}
	DirectX::XMFLOAT3 vec;
	vec.x = position.x + ax;
	vec.z = position.z + 1.0f;
	//Y軸方向には発動しない
	vec.y = 0.0f;

	return vec;
}

//移動入力処理
bool Player::InputMove(float elapsedTime)
{
	if (jumpCT > 0.0f)
	{
		jumpCT -= 1.0f * elapsedTime;
	}
	moveSpeed += Velocity * elapsedTime;
	if (moveSpeed >= maxSpeed && jumpFlag == false)
	{
		moveSpeed = maxSpeed;
	}

	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	float vx = moveVec.x - position.x;
	float vz = moveVec.z - position.z;
	float dist = sqrtf(vx * vx + vz * vz);
	vx /= dist;
	vz /= dist;

	if (vx != 0.0f && moveSpeed >= maxSpeed)
	{
		moveSpeed = 3.1f;
	}
	
	//進行方向に応じて押す向きを設定
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
	Enemy& enemy = Enemy::Instance();
	//衝突処理
	DirectX::XMFLOAT3 outPosition;
	if (Collision::IntersectCylinderVsCylinder(
		position,
		radius,
		height,
		enemy.GetPosition(),
		enemy.GetRadius(),
		enemy.GetHeight(),
		outPosition
	))
	{
		DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy.GetPosition());
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

			if (jumpFlag && !enemy.GetJump())
			{
				Hit->Play(false);//攻撃のヒット音再生
				outPosition.x = position.x + (2.0f * L);
				enemy.SetPosition(outPosition);
			}
			else if (power != 0.0f && enemy.GetPower() * -1.0f == power)
			{
				outPosition.x = position.x + ((enemy.GetRadius() * 0.5f) * L);
				enemy.SetPosition(outPosition);
			}
			else if (power != 0.0f && enemy.GetPower() == 0.0f)
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				enemy.SetPosition(outPosition);
			}
			else
			{
				Hit->Play(false);
				outPosition.x = position.x + (2.0f * L);
				enemy.SetPosition(outPosition);
			}
		}
	}
}

//ジャンプ入力処理
bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisLY();
	if (ay >= 0.1f)
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
			if (muluchmode != true)
			{
				DirectX::XMFLOAT3 Epos = Enemy::Instance().GetPosition();
				if (position.x - Epos.x > 0.0f)
				{
					power = 1.0f;
				}
				else if (position.x - Epos.x < 0.0f)
				{
					power = -1.0f;
				}
			}
			//ジャンプ入力した
			return true;
		}		
	}
	return false;
}

//着地した際に呼ばれる
void Player::OnLanding()
{
	if (jumpFlag == true)
	{
		JumpEnd->Play(false);
		jumpCT = 1.0f;
		jumpFlag = false;
		jumpCount = 0;
		if (moveSpeed >= 1.5f)
		{
			moveSpeed /= 2.0f;
		}
	}

	if (velocity.y < -10.0f)
	{
		if (state != State::Damage && state != State::Death)
		{
			TransitionMoveState();			
		}
	}
}

//攻撃入力処理
//bool Player::InputAttack()
//{
//	GamePad& gamePad = Input::Instance().GetGamePad();
//	float ay = gamePad.GetAxisLY();
//	if(ay > 0.1f)
//	{
//		return true;
//	}
//	return false;
//}

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

	////ジャンプ入力処理
	//if (InputJump())
	//{
	//	TransitionJumpFlipState();
	//}
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

////2段ジャンプステートへ遷移
//void Player::TransitionJumpFlipState()
//{
//	state = State::JumpFlip;
//
//	//ジャンプアニメーション再生
//	model->PlayAnimation(Anim_Jump_Flip, false);
//}
//
////2段ジャンプステート更新処理
//void Player::UpdateJumpFlipState(float elapsedTime)
//{
//	InputMove(elapsedTime);
//}
//
////攻撃ステートへ遷移
//void Player::TransitionAttackState()
//{
//	state = State::Attack;
//
//	//攻撃アニメーション再生
//	//model->PlayAnimation(Anim_Attack, false);
//}
//
////攻撃ステート更新処理
//void Player::UpdateAttackState(float elapsedTime)
//{
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleState();
//	}
//	//任意のアニメーション再生区間でのみ当たり判定を発生させる
//	float animetionTime = model->GetCurrentAnimetionSeconds();
//	if (animetionTime > 0.3f && animetionTime < 0.4f)
//	{
//		attackColisionFlag = true;
//	}
//	else attackColisionFlag = false;
//
//	if (attackColisionFlag)
//	{
//		//左手ノードとエネミーの衝突処理
//		CollisionNodeVsEnemies("mixamorig:LeftHand", leftHandRadius);
//	}
//}
//
////ノードと敵の衝突処理
//void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
//{
//	//ノード取得
//	Model::Node* node = model->FindNode(nodeName);
//	//ノード位置取得
//	DirectX::XMFLOAT3 nodePosition;
//	nodePosition.x = node->worldTransform._41;
//	nodePosition.y = node->worldTransform._42;
//	nodePosition.z = node->worldTransform._43;
//
//	Enemy& enemy = Enemy::Instance();
//
//	//衝突処理
//	DirectX::XMFLOAT3 outPosition;
//	if (Collision::IntersectSpherVsCyling(
//		nodePosition,
//		nodeRadius,
//		enemy.GetPosition(),
//		enemy.GetRadius(),
//		enemy.GetHeight(),
//		outPosition
//	))
//		//ダメージを与える
//		if (enemy.ApplyDamage(1, 0.5f))
//		{
//			//吹き飛ばす
//			{
//				DirectX::XMFLOAT3 impulse;
//				const float power = 10.0f;//吹き飛ばす力
//
//				//敵の位置を取得
//				const DirectX::XMFLOAT3& e = enemy.GetPosition();
//				//弾の位置を取得
//				const DirectX::XMFLOAT3& p = nodePosition;
//				//弾から敵へのベクトルを取得
//				float vx = e.x - p.x;
//				float vz = e.z - p.z;
//				//そのベクトルを正規化(長さを計算し、長さで割る)
//				float lengthXZ = sqrtf(vx * vx + vz * vz);
//				vx /= lengthXZ;
//				vz /= lengthXZ;
//				//衝撃の値を設定(xzは正規化したベクトルをpower文スケーリング)
//				impulse.x = vx * power;
//				impulse.y = power * 0.5f;
//				impulse.z = vz * power;
//
//				enemy.AddImpulse(impulse);
//			}
//			//ヒットエフェクト再生
//			{
//				DirectX::XMFLOAT3 e = enemy.GetPosition();
//				e.y += enemy.GetHeight() * 0.5f;
//				hitEffect->Play(e);
//			}
//		}
//}
//
////ダメージステートへ遷移
//void Player::TransitionDamageState()
//{
//	state = State::Damage;
//
//	//ダメージアニメーション再生
//	model->PlayAnimation(Anim_GetHit1, false);
//}
//
////ダメージステート更新処理
//void Player::UpdateDamageState(float elapsedTime)
//{
//	//ダメージアニメーション終了後に待機ステートへ遷移
//	if (!model->IsPlayAnimation())
//	{
//		TransitionIdleState();
//	}
//}
//
////死亡ステートへ遷移
//void Player::TransitionDeathState()
//{
//	state = State::Death;
//
//	//ダメージアニメーション再生
//	model->PlayAnimation(Anim_Death, false);
//}
//
////死亡ステート更新処理
//void Player::UpdateDeathState(float elapsedTime)
//{
//	if (!model->IsPlayAnimation())
//	{
//		//ボタンを押したら復活
//		GamePad& gamePad = Input::Instance().GetGamePad();
//		if (gamePad.GetButtonDown() & GamePad::BTN_A)
//		{
//			TransitionReviveState();
//		}
//	}
//}
//
////復活ステートへ遷移
//void Player::TransitionReviveState()
//{
//	state = State::Revive;
//
//	//復活アニメーション再生
//	model->PlayAnimation(Anim_Revive, false);
//}
//
////復活ステート更新処理
//void Player::UpdateReviveState(float elapsedTime)
//{
//	//復活アニメーション終了後に待機ステートへ遷移
//	if (!model->IsPlayAnimation())
//	{
//		health = 5;
//		invincibleTimer = 1.0f;
//		TransitionIdleState();
//	}
//}