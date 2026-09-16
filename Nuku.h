#pragma once

// 構造体宣言
struct OBJECT // プレイヤーと敵用
{
	float x; // x座標
	float y; // y座標
	int imageNum; // 画像の番号
	int timer; // タイマー
	int interval; // フレーム間隔
	int speed; // 速度
	int distance; // 距離
	int spTimer; // 速度管理タイマー
};

// 関数プロトタイプ宣言
void InitGame(void); // 初期化用の関数
void InitVariable(void); // ゲーム開始時の初期値
void ScrollBG(int spd); // 背景のスクロール関数
void ScrollWY(int spd); // 道路のスクロール関数
void PlayerSpeed(void); // プレイヤーの速度用関数
void Player(void); // プレイヤー描画用の関数
void Text(void); // テキストの表示
void Enemy(void); // 敵の描画
void EnemySpeed(void); // 敵の速度管理
void EnemyDistance(void); // 敵の位置
void Distance(void); // ゴールまでの残り距離
void Goal(void); // ゴール
void Win(void); // 勝ちシーン
void Lose(void); // 負けシーン
void Title(void); // タイトルシーン
void TitleCar(void); // タイトルシーンの車用関数
void Explanation(void); // ゲーム説明