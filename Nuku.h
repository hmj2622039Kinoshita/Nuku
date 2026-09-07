#pragma once

// 構造体宣言
struct OBJECT // プレイヤーと敵用
{
	float x; // x座標
	float y; // y座標
	float vx; // x方向の速さ
	float speed; // 加速度
};

// 関数プロトタイプ宣言
void initGame(void); // 初期化用の関数
void scrollBG(int spd); // 背景のスクロール関数
void InitVariable(void); // ゲーム開始時の初期値