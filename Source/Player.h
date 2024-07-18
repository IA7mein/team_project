#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Effect.h"

//プレイヤー
class Player :public Character
{
public:
	Player();
	~Player() override;

	//インスタンス取得
	static Player& Instance();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//デバッグプリミティブ描画
	void DrawDebugPrimitive();

	//ジャンプ入力処理
	bool InputJump();

	//入力状態取得
	float GetPower() const { return power; }

	//ジャンプ状態取得
	bool GetJump() const { return jumpFlag; };
	
protected:
	//着地した際に呼ばれる
	void OnLanding() override;

	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	//移動入力処理
	bool InputMove(float elapsedTime);

	//プレイヤーとエネミーの衝突判定
	void CollisionPlayerVsEnemies();

	////ダメージを受けた際に呼ばれる
	//void OnDamaged() override;

	////死亡した際に呼ばれる
	//void OnDead() override;

private:
	////攻撃入力処理
	//bool InputAttack();

	//待機ステートへ遷移
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	//移動ステートへ遷移
	void TransitionMoveState();

	//移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	//ジャンプステートへ遷移
	void TransitionJumpState();

	//ジャンプステート更新処理
	void UpdateJumpState(float elapsedTime);

	//着地ステートへ遷移
	void TransitionLandState();

	//着地ステート更新処理
	void UpdateLandState(float elapsedTime);

	////2段ジャンプステートへ遷移
	//void TransitionJumpFlipState();

	////2段ジャンプステート更新処理
	//void UpdateJumpFlipState(float elapsedTime);

	////攻撃ステートへ遷移
	//void TransitionAttackState();

	////攻撃ステート更新処理
	//void UpdateAttackState(float elapsedTime);

	////ノードとエネミーの衝突判定
	//void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	////ダメージステートへ遷移
	//void TransitionDamageState();

	////ダメージステート更新処理
	//void UpdateDamageState(float elapsedTime);

	////死亡ステートへ遷移
	//void TransitionDeathState();

	////死亡ステート更新処理
	//void UpdateDeathState(float elapsedTime);

	////復活ステートへの遷移
	//void TransitionReviveState();

	////復活ステート更新処理
	//void UpdateReviveState(float elapsedTime);

private:
	//ステート
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		JumpFlip,
		Attack,
		Damage,
		Death,
		Revive
	};
	//アニメーション
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_Jump,
		Anim_Jump_Flip,
		Anim_Landing,
		Anim_Revive,
		Anim_Running,
		Anim_Walking
	};

	Model* model = nullptr;
	float moveSpeed = 0.0f;
	float maxSpeed = 2.0f;
	float Velocity = 0.5f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	float jumpSpeed = 20.0f;
	int jumpCount = 0;
	int jumpLimit = 1;
	
	Effect* hitEffect = nullptr;
	Effect* hart = nullptr;
	Effect* shield = nullptr;
	State state = State::Idle;
	float leftHandRadius = 0.4f;
	bool attackColisionFlag = false;
	bool jumpFlag = false;
	float jumpCT = 0.0f;
	float power = 0.0f;

	//オーディオリソース
	std::unique_ptr<AudioSource>JumpStart;//ジャンプ入力SE
	std::unique_ptr<AudioSource>JumpEnd;//着地のSE
	std::unique_ptr<AudioSource>Hit;//敵の攻撃に当たった際のSE
	std::unique_ptr<AudioSource>ItemGet;//アイテム、スコア入手SE
	std::unique_ptr<AudioSource>HitObject;//障害物ヒットSE
};