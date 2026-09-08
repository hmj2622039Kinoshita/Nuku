#include"DxLib.h"
#include"Nuku.h" // ヘッダーファイル

//定数定義 
const int WIDTH = 1536, HEIGHT = 896; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート

// 構造体
struct OBJECT player; // プレイヤーの構造体変数

// グローバル変数
int imgBackGrass; // 背景画像（草）
int imgPlayer[2]; // プレイヤー画像
int imgWayGrass1, imgWayGrass2; // 道路画像（草）
int timer; // 車のアニメーションの速さ管理するためのタイマー


// グローバル関数
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("Nuku Game"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化、エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏背景にする
	
	InitGame(); // 初期化用の関数を呼び出す
	InitVariable(); // ゲーム開始時の初期値
	
	while (1)
	{
		ClearDrawScreen(); // 画面をクリアにする

		// ゲームの骨組み処理
		ScrollBG(1); // 背景のスクロール
		ScrollWY(1); // 道路のスクロール
		Player(); // プレイヤー




		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(1000 / FPS); // 一定時間待つ
		if (ProcessMessage() == -1) break; // windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}
	DxLib_End(); // DXライブラリの使用終了処理
	return 0; // ソフトの終了
}

// 自作関数記述
// 初期化用の関数
void InitGame(void)
{
	// 画像の読み込み
	imgBackGrass = LoadGraph("Material/Background/Grass.png"); //背景画像（草）
	imgPlayer[0] = LoadGraph("Material/Robot/green1.png"); // プレイヤー画像
	imgPlayer[1] = LoadGraph("Material/Robot/green2.png");
	imgWayGrass1 = LoadGraph("Material/Way/grass.png"); // 道路画像（草）
	imgWayGrass2 = LoadGraph("Material/Way/grass2.png"); 
}

// ゲーム開始時の初期値
void InitVariable(void)
{
	// プレイヤーの構造体
	player.x = WIDTH / 2 - 79; // プレイヤーの左上のX座標
	player.y = HEIGHT - 140 - 118; // プレイヤーの左上のY座標
	player.speed = 1; // プレイヤーのスピード（背景、道路、アニメーション、敵の車もこれで管理する）
}

// 背景のスクロール
void ScrollBG(int spd)
{
	static int backGrassX; // スクロールの位置を管理する変数
	backGrassX = (backGrassX - spd) % 1024; // 背景（草）のX座標
	DrawGraph(backGrassX, -64, imgBackGrass, false); // 背景（草）の描画
	DrawGraph(backGrassX + 1024, -64, imgBackGrass, false);
	DrawGraph(backGrassX + 2048, -64, imgBackGrass, false);
}

// 道路のスクロール
void ScrollWY(int spd)
{
	static int wayGrassX; // スクロールの位置を管理する変数
	wayGrassX = (wayGrassX - spd) % 70; // 道路（草）のX座標
	for (int i = 0; i < 24; i++)
	{
		DrawGraph(wayGrassX + i * 70, HEIGHT - 70 * 2, imgWayGrass1, true); // 道路（草）の描画
		DrawGraph(wayGrassX + i * 70, HEIGHT - 70 , imgWayGrass2, true);
	}
}

// プレイヤー
void Player(void)
{
	if (player.speed == 0)
	{
		DrawGraph(player.x, player.y, imgPlayer[0], true);
	}
	else
	{
		timer++; //	アニメーションタイマー動かす
		if (timer <= 10)
		{
			DrawGraph(player.x, player.y, imgPlayer[1], true);
		}
		else if (10 < timer && timer <= 20)
		{
			DrawGraph(player.x, player.y, imgPlayer[0], true);
		}
		else if (20 < timer)
		{
			DrawGraph(player.x, player.y, imgPlayer[1], true);
			timer = 0; // タイマーリセット
		}

	}
}