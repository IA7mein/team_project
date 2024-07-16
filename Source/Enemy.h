#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "Effect.h"

//エネミー
class Enemy : public Character
{
public:
	Enemy();
	~Enemy()override;

	//インスタンス取得
	static Enemy& Instance();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//デバッグプリミティブ描画
	void DrawDebugPrimitive();

	//縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	//入力情報取得
	float GetPower() const { return power; };

	bool GetJump() const { return jumpFlag; };

	void eJump();

	//ターゲット位置をランダムに設定
	void SetRandomTargetPosition();

protected:
	//着地した際に呼ばれる
	void OnLanding() override;

	////死亡したときに呼ばれる
	//void OnDead() override;

	////ダメージを受けた際に呼ばれる
	//void OnDamaged() override;

	void CollisionPlayerVsEnemies();

private:
	//目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);

	//プレイヤー索敵
	bool SearchPlayer();

	//ノードとプレイヤーの衝突処理
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	//徘徊ステートへ遷移
	void TransitionWanderState();

	//徘徊ステート更新処理
	void UpdateWanderState(float elapsedTime);

	//待機ステートへ遷移
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	////追跡ステートへ遷移
	//void TransitionPursuitState();

	////追跡ステート更新処理
	//void UpdatePursuitState(float elapsedTime);

	////攻撃ステートへ遷移
	//void TransitionAttackState();

	////攻撃ステート更新処理
	//void UpdateAttackState(float elapsedTime);

	////戦闘待機ステートへ遷移
	//void TransitionIdleBattleState();

	////戦闘待機ステートへ更新処理
	//void UpdateIdleBattleState(float elapsedTime);

	////ダメージステートへ遷移
	//void TransitionDamageState();

	////ダメージステート更新処理
	//void UpdateDamageState(float elapsedTime);

	////死亡ステートへ遷移
	//void TransitionDeathState();

	////死亡ステート更新処理
	//void UpdateDeathState(float elapsedTime);

private:
	//ステート
	enum class State
	{
		Wander,
		Idle,
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death
	};
	Model* model = nullptr;
	State state = State::Wander;
	DirectX::XMFLOAT3 targetPosition = { 1, 0, -7 };
	DirectX::XMFLOAT3 territoryOrigin = { 0, 0, 0};
	float territoryRange = 10.0f;
	float moveSpeed = 0.0f;
	float maxSpeed = 2.5f;
	float Velocity = 0.5f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float stateTimer = 0.0f;
	float searchRange = 3.0f;
	float attackRange = 1.5f;
	int targetRandomX = 2;//X軸ランダム値
	int jumpCount = 0;
	int jumpLimit = 1;
	float jumpSpeed = 20.0f;
	bool jumpFlag = false;
	float jumpCT = 0.0f;
	float power = 0.0f;

	//オーディオリソース
	std::unique_ptr<AudioSource>JumpStart;//ジャンプ入力時のSE
	std::unique_ptr<AudioSource>JumpEnd;//着地SE
	std::unique_ptr<AudioSource>Hit;//敵に当たった際のSE
	std::unique_ptr<AudioSource>ItemGet;//アイテム、スコア入手SE
	std::unique_ptr<AudioSource>HitObject;//障害物ヒットSE
};