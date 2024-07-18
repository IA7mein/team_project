#include <imgui.h>
#include "Player.h"
#include "Player2.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "Collision.h"
#include "SceneTitle.h"
static Player2* instance = nullptr;
bool goalP2;
//インスタンス取得
Player2& Player2::Instance()
{
	return *instance;
}

//コンストラクタ
Player2::Player2()
{
	//インスタンスポインタ設定
	instance = this;

	model = new Model("Data/Model/Player/2P.mdl");

	JumpStart = Audio::Instance().LoadAudioSource("Data/SE/JumpStart.wav");
	JumpEnd = Audio::Instance().LoadAudioSource("Data/SE/JumpEnd.wav");
	Hit = Audio::Instance().LoadAudioSource("Data/SE/Hit.wav");
	ItemGet = Audio::Instance().LoadAudioSource("Data/SE/ItemGet.wav");
	HitObject = Audio::Instance().LoadAudioSource("Data/SE/HitObject.wav");

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.5f;

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
Player2::~Player2()
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
void Player2::Update(float elapsedTime)
{
	if (position.y < -10.0f)position = { 0, 1, -10 };

	if (position.z >= 78.0f)
	{
		goalP2 = true;
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

	//プレイヤーとプレイヤーの衝突判定
	if (muluchmode)//ここに;
	{
		CollisionPlayerVsPlayer();
	}

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
void Player2::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc, model);
}

//デバッグプリミティブ描画
void Player2::DrawDebugPrimitive()
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
DirectX::XMFLOAT3 Player2::GetMoveVec() const
{
	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
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
bool Player2::InputMove(float elapsedTime)
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
	return false;

}

//プレイヤーとエネミーとの衝突判定
void Player2::CollisionPlayerVsPlayer()
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

			if (power != 0)
			{
				if (jumpFlag && !player.GetJump())
				{
					Hit->Play(false);//攻撃のヒット音再生
					outPosition.x = position.x + (5.0f * L);
					player.SetPosition(outPosition);
				}
				else if (power != 0.0f && player.GetPower() * -1.0f == power)
				{
					outPosition.x = position.x + ((player.GetRadius()) * L);
					player.SetPosition(outPosition);
				}
				else if (power != 0.0f && player.GetPower() == 0.0f)
				{
					Hit->Play(false);
					outPosition.x = position.x + (5.0f * L);
					player.SetPosition(outPosition);
				}
				else
				{
					Hit->Play(false);
					outPosition.x = position.x + (5.0f * L);
					player.SetPosition(outPosition);
				}
			}
		}
	}
}

//ジャンプ入力処理
bool Player2::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisRY();
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
			//ジャンプ入力した
			return true;
		}
	}
	return false;
}

//着地した際に呼ばれる
void Player2::OnLanding()
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

//待機ステートへ遷移
void Player2::TransitionIdleState()
{
	state = State::Idle;

	//待機アニメーション再生
	model->PlayAnimation(Anim_Idle, true);
}

//待機ステート更新処理
void Player2::UpdateIdleState(float elapsedTime)
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

void Player2::TransitionMoveState()
{
	state = State::Move;

	//走りアニメーション再生
	model->PlayAnimation(Anim_Running, true);
}

//移動ステート更新処理
void Player2::UpdateMoveState(float elapsedTime)
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
void Player2::TransitionJumpState()
{
	state = State::Jump;

	//ジャンプアニメーション再生
	model->PlayAnimation(Anim_Jump, false);
}

//ジャンプステート更新処理
void Player2::UpdateJumpState(float elapsedTime)
{
	InputMove(elapsedTime);

	////ジャンプ入力処理
	//if (InputJump())
	//{
	//	TransitionJumpFlipState();
	//}
}

//着地ステートへ遷移
void Player2::TransitionLandState()
{
	state = State::Land;

	//ジャンプアニメーション再生
	model->PlayAnimation(Anim_Landing, false);
}

//着地ステート更新処理
void Player2::UpdateLandState(float elapsedTime)
{
	if (!model->IsPlayAnimation())
	{
		TransitionIdleState();
	}
}
