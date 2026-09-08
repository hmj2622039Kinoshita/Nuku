#pragma once

// 構造体宣言
struct OBJECT // プレイヤーと敵用
{
	float x; // x座標
	float y; // y座標
	float speed; // 速度
};

// 関数プロトタイプ宣言
void InitGame(void); // 初期化用の関数
void InitVariable(void); // ゲーム開始時の初期値
void ScrollBG(int spd); // 背景のスクロール関数
void ScrollWY(int spd); // 道路のスクロール関数
void Player(void); // プレイヤー用の関数