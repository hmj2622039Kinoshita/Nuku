#pragma once

// 構造体宣言
struct OBJECT // プレイヤーと敵用
{
	float x; // x座標
	float y; // y座標
	int imageNum; // 画像の番号
	int timer; // タイマー
	int interval; // フレーム間隔
};

// 関数プロトタイプ宣言
void InitGame(void); // 初期化用の関数
void InitVariable(void); // ゲーム開始時の初期値
void ScrollBG(int spd); // 背景のスクロール関数
void ScrollWY(int spd); // 道路のスクロール関数
void PlayerSpeed(void); // プレイヤーの速度用関数
void Player(void); // プレイヤー描画用の関数
void Distance(void); // ゴールまでの残り距離