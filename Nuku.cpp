#include"DxLib.h"
#include"Nuku.h" // ヘッダーファイル

//定数定義 
const int WIDTH = 1536, HEIGHT = 896; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート

// グローバル変数
int imgBackGrass; // 背景画像（草）

// グローバル関数
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("Nuku Game"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化、エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏背景にする
	
	initGame(); // 初期化用の関数を呼び出す
	InitVariable(void); // ゲーム開始時の初期値
	
	while (1)
	{
		ClearDrawScreen(); // 画面をクリアにする

		// ゲームの骨組み処理
		scrollBG(1); // 背景のスクロール,プレイヤーの速さと連動させる


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
void initGame(void)
{
	// 背景画像の読み込み
	imgBackGrass = LoadGraph("Material/Background/Grass.png"); //背景画像（草）
}

// 背景のスクロール
void scrollBG(int spd)
{
	static int backGrassX; // スクロールの位置を管理する変数
	backGrassX = (backGrassX - spd) % WIDTH; // 背景（草）のX座標
	DrawGraph(backGrassX, -64, imgBackGrass, false); // 背景（草）の描画
	DrawGraph(backGrassX + 1024, -64, imgBackGrass, false);
}