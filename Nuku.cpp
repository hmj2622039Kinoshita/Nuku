#include"DxLib.h"
#include"Nuku.h" // ヘッダーファイル
#include<time.h>

//定数定義 
const int WIDTH = 1536, HEIGHT = 896; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート

// 構造体
struct OBJECT player; // プレイヤーの構造体変数
struct OBJECT enemy; // 敵の構造体

// グローバル変数
int imgBackGrass; // 背景画像（草）
int imgPlayer[2]; // プレイヤー画像
int imgWayGrass1, imgWayGrass2; // 道路画像（草）
int imgEnemy[2]; // 敵画像
int speed = 100; // プレイヤーの速度
int keyTimer; // キー入力タイマー
int keyStep = 0; // 0=左,1=上,2=右、過去のキーを覚える
int distance; // ゴールまでの残り距離
int startTimer = 0; // 開始からのタイマー
int playerMove = 0; // プレイヤーの移動総距離
int enemyMove  = 0; // 敵の移動総距離

// グローバル関数
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("Nuku Game"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化、エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏背景にする
	
	// 距離のランダム値
	srand((static_cast<int>(time(NULL)))); // ランダムシード
	distance = rand() % 8001;// ゴールまでの残り距離
	InitGame(); // 初期化用の関数を呼び出す
	InitVariable(); // ゲーム開始時の初期値
	
	while (1)
	{
		ClearDrawScreen(); // 画面をクリアにする

		// ゲームの骨組み処理
		ScrollBG(speed / 10); // 背景のスクロール
		ScrollWY(speed / 10); // 道路のスクロール
		PlayerSpeed(); // プレイヤーの速度（キー入力）
		EnemyDistance(); // 敵の位置
		Enemy(); // 敵の描画
		Player(); // プレイヤーの描画
		Distance(); // 残り距離
		Goal(); // ゴール

		


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
	imgEnemy[0] = LoadGraph("Material/Robot/yellow1.png"); // 敵画像
	imgEnemy[1] = LoadGraph("Material/Robot/yellow2.png");
}

// ゲーム開始時の初期値
void InitVariable(void)
{
	// プレイヤーの構造体
	player.x = WIDTH / 2 - 79; // プレイヤーの左上のX座標
	player.y = HEIGHT - 140 - 120; // プレイヤーの左上のY座標
	player.imageNum = 0; // プレイヤーの画像の番号
	player.timer = 0; // タイマー
	player.interval = 0; // フレームの間隔
	// 敵の構造体
	enemy.x = WIDTH / 2 - 92; // 敵の左上のX座標
	enemy.y = HEIGHT - 140 - 150; // 敵の左上のY座標
	enemy.imageNum = 0; // 敵の画像番号
	enemy.timer = 0; // 敵タイマー
	enemy.interval = 0; // 敵のフレーム間隔
	enemy.speed = 0; // 敵の速度
	enemy.spTimer; // 敵のスピード管理用タイマー
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

// プレイヤーの速度
void PlayerSpeed(void)
{
	static int oldLeft = 0; // 左キーの記録
	static int oldUp = 0; // 上キーの記録
	static int oldRight = 0; // 右キーの記録
	int left = CheckHitKey(KEY_INPUT_LEFT); // 左キー
	int up = CheckHitKey(KEY_INPUT_UP); // 上キー
	int right = CheckHitKey(KEY_INPUT_RIGHT); // 右キー
	keyTimer++;
	if (left == 1 && oldLeft == 0) // 左キーが押されていなくて押された
	{
		if (keyStep == 0)
		{
			keyStep = 1; // 上
			keyTimer = 0;
		}
		else
		{
			if (speed > 0)
			{
				speed -= 10;
			}
			else
			{
				speed = 0;
			}
			keyStep = 0; // 左
			keyTimer = 0;
		}
	}
	if (up == 1 && oldUp == 0) // 上キーが押されてなくて押された
	{
		if (keyStep == 1) // 上
		{
			keyStep = 2; // 右
			keyTimer = 0;
		}
		else
		{
			if (speed > 0)
			{
				speed -= 10;
			}
			else
			{
				speed = 0;
			}
			keyStep = 0; // 左
			keyTimer = 0;
		}
	}
	if (right == 1 && oldRight == 0) // 右キーが押されてなくて押された
	{
		if (keyStep == 2) // 右
		{
			if (speed < 200)
			{
				speed += 10;
			}
			else
			{
				speed = 200;
			}
			keyStep = 0; // 左
			keyTimer = 0;
		}
		else
		{
			if (speed > 0)
			{
				speed -= 10;
			}
			keyStep = 0; // 左
			keyTimer = 0;
		}
	}
	if (keyTimer > 60) // 何も入力がなかったら
	{
		if (speed > 0)
		{
			speed -= 10;
		}
		else
		{
			speed = 0;
		}
		keyStep = 0;
		keyTimer = 0;
	}
	oldLeft = left;
	oldUp = up;
	oldRight = right;
	// 速度表示
	SetFontSize(30);
	DrawFormatString(WIDTH - 360, 85, GetColor(0, 0, 0), "現在のスピード　%dkm", speed);
}

// プレイヤーの描画
void Player(void)
{
	player.timer++;
	if (speed == 0) // 速度０の時
	{
		DrawGraph(player.x, player.y, imgPlayer[0], true);
	}
	else // 速度が０より大きいとき
	{
		player.interval = 23 - speed / 10; // スピードごとのフレーム間隔
		if (player.timer >= player.interval) // 一定フレームが経過した
		{
			player.timer = 0;
			if (player.imageNum == 0) // 画像番号の切り替え
			{
				player.imageNum = 1;
			}
			else
			{
				player.imageNum = 0;
			}
		}
	}
	if (player.imageNum == 0) // 画像番号でプレイヤーの描画
	{
		DrawGraph(player.x, player.y, imgPlayer[0], true);
	}
	else
	{
		DrawGraph(player.x, player.y + 4, imgPlayer[1], true);
	}
}

// 敵の描画
void Enemy(void)
{
	// 徐々にスピード上げる
	enemy.spTimer++;
	if (enemy.spTimer > 60)
	{
		enemy.speed += 10;
		enemy.spTimer = 0; // タイマーリセット
	}
	// 敵のアニメーション
	enemy.timer++;
	if (enemy.speed == 0) // 速度０の時
	{
		DrawGraph(enemy.x, enemy.y, imgEnemy[0], true);
	}
	else // 速度が０より大きいとき
	{
		enemy.interval = 23 - enemy.speed / 10; // スピードごとのフレーム間隔
		if (enemy.timer >= enemy.interval) // 一定フレームが経過した
		{
			enemy.timer = 0;
			if (enemy.imageNum == 0) // 画像番号の切り替え
			{
				enemy.imageNum = 1;
			}
			else
			{
				enemy.imageNum = 0;
			}
		}
	}
	if (enemy.imageNum == 0) // 画像番号でプレイヤーの描画
	{
		DrawGraph(enemy.x, enemy.y, imgEnemy[0], true);
	}
	else
	{
		DrawGraph(enemy.x, enemy.y + 4, imgEnemy[1], true);
	}
	if (enemy.speed > 110)
	{
		enemy.speed = 110;
	}

	SetFontSize(30);
	DrawFormatString(WIDTH /2, 85, GetColor(0, 0, 0), "現在のスピード　%dkm", enemy.speed);
}

// 敵の位置
void EnemyDistance(void)
{
	float space; // 二台の間の距離
	playerMove += startTimer * speed;
	enemyMove += startTimer * enemy.speed;
	space = abs(playerMove - enemyMove);
	if (playerMove > enemyMove)
	{
		enemy.x -= space / 60;
	}
	if (playerMove < enemyMove)
	{
		enemy.x += space / 60;
	}
}

// ゴールまでの距離
void Distance(void)
{
	SetFontSize(30);
	DrawFormatString(WIDTH -200, 50, GetColor(0, 0, 0), "残り %dｍ", distance);
}

// ゴール
void Goal(void)
{
	
}