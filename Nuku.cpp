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
int scene = 1; // シーン
int imgBackGrass; // 背景画像（草）
int imgBackEmpty; // 背景画像（空）
int imgPlayer[3]; // プレイヤー画像
int imgWayGrass1, imgWayGrass2; // 道路画像（草）
int imgEnemy[3]; // 敵画像
int imgBlue[2]; // 青の車の画像
int imgRed[2]; // 赤の車の画像
int imgGoal; // ゴール画像
int imgCrown; // 王冠画像
int speed = 0; // プレイヤーの速度
int keyTimer; // キー入力タイマー
int startTimer = 0; // 開始用タイマー
int sceneTimer = 0; // シーン遷移用タイマー
int titleTimer = 0; // タイトル用タイマー
int carTimer = 0; // タイトルの車用タイマー
int keyStep = 0; // 0=左,1=上,2=右、過去のキーを覚える
int distance; // ゴールまでの残り距離
int playerMove = 0; // プレイヤーの移動総距離
int enemyMove  = 0; // 敵の移動総距離
int countDown; // 開始までのカウントダウン
int imageNum = 0; // タイトル用車の画像番号
bool enemySpeed = true; // 敵の速度の増加管理

// グローバル関数
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("Nuku Game"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化、エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏背景にする
	
	while (1)
	{
		ClearDrawScreen(); // 画面をクリアにする

		// ゲームの骨組み処理
		switch (scene)
		{
		case 1: // タイトルシーン
			// 距離のランダム値
			srand((static_cast<int>(time(NULL)))); // ランダムシード
			distance = rand() % 7001 + 1000;// ゴールまでの残り距離
			InitGame(); // 初期化用の関数を呼び出す
			InitVariable(); // ゲーム開始時の初期値
			ScrollBG(0); // 背景
			ScrollWY(0); // 道路のスクロール
			Title(); // タイトル
			TitleCar(); // 車用関数
			break;
		case 2: // ゲーム説明シーン
			Explanation(); // ゲーム説明
			break;
		case 3: // プレイシーン
			ScrollBG(speed / 10); // 背景のスクロール
			ScrollWY(speed / 10); // 道路のスクロール
			PlayerSpeed(); // プレイヤーの速度（キー入力）
			EnemyDistance(); // 敵の位置
			Enemy(); // 敵の描画
			EnemySpeed(); // 敵の速度
			Player(); // プレイヤーの描画
			Distance(); // 残り距離
			Text(); // テキストの表示
			Goal(); // ゴール
			break;
		case 4: // プレイヤーの勝ち
			ScrollBG(1); // 背景のスクロール
			ScrollWY(0); // 道路のスクロール
			Win(); // 勝ち
			break;
		case 5: // プレイヤーの負け
			ScrollBG(1); // 背景のスクロール
			ScrollWY(0); // 道路のスクロール
			Lose(); // 負け
			break;
		}
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
	imgBackEmpty = LoadGraph("Material/Background/Empty.png"); // 背景画像（空）
	imgPlayer[0] = LoadGraph("Material/Robot/green1.png"); // プレイヤー画像
	imgPlayer[1] = LoadGraph("Material/Robot/green2.png");
	imgPlayer[2] = LoadGraph("Material/Robot/greenJump.png"); // プレイヤー画像（勝ちシーン用）
	imgWayGrass1 = LoadGraph("Material/Way/grass.png"); // 道路画像（草）
	imgWayGrass2 = LoadGraph("Material/Way/grass2.png");
	imgEnemy[0] = LoadGraph("Material/Robot/yellow1.png"); // 敵画像
	imgEnemy[1] = LoadGraph("Material/Robot/yellow2.png");
	imgEnemy[2] = LoadGraph("Material/Robot/yellowJump.png"); // 敵画像（負けシーン用）
	imgBlue[0] = LoadGraph("Material/Robot/blue1.png"); // 青の車の画像
	imgBlue[1] = LoadGraph("Material/Robot/blue2.png");
	imgRed[0] = LoadGraph("Material/Robot/red1.png"); // 赤の車の画像
	imgRed[1] = LoadGraph("Material/Robot/red2.png");
	imgGoal = LoadGraph("Material/flag.png"); // ゴール画像
	imgCrown = LoadGraph("Material/crown.png"); // 王冠画像
}

// ゲーム開始時の初期値
void InitVariable(void)
{
	// プレイヤーの構造体
	player.x = WIDTH / 2 - 79; // プレイヤーの左上のX座標
	player.y = HEIGHT - 260; // プレイヤーの左上のY座標
	player.imageNum = 0; // プレイヤーの画像の番号
	player.timer = 0; // タイマー
	player.interval = 0; // フレームの間隔
	// 敵の構造体
	enemy.x = WIDTH / 2 - 92; // 敵の左上のX座標
	enemy.y = HEIGHT - 290; // 敵の左上のY座標
	enemy.imageNum = 0; // 敵の画像番号
	enemy.timer = 0; // 敵タイマー
	enemy.interval = 0; // 敵のフレーム間隔
	enemy.speed = 0; // 敵の速度
	enemy.spTimer = 0; // 敵のスピード管理用タイマー

	countDown = 5; // カウントダウン
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
}

// テキストの表示
void Text(void)
{
	SetFontSize(30);
	// ゴールまでの残り距離表示
	DrawFormatString(WIDTH - 195, 50, GetColor(0, 0, 0), "残り %3dｍ", distance);
	// 速度表示
	DrawFormatString(WIDTH - 351, 85, GetColor(0, 0, 0), "現在のスピード %3dｍ", speed);
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
	if (enemy.speed > 120) // 最高速度
	{
		enemy.speed = 120;
	}
}

// 敵の速度
void EnemySpeed(void)
{
	// 徐々にスピード上げる
	if (enemySpeed == true)
	{
		enemy.spTimer++;
		if (enemy.spTimer > 60) // 1秒ごとに速度10増加
		{
			enemy.speed += 10;
			enemy.spTimer = 0; // タイマーリセット
		}
	}
}

// 敵の位置
void EnemyDistance(void)
{
	float speedDis; // プレイヤーと敵の速度差
	speedDis = speed - enemy.speed;
	enemy.x -= speedDis / 10; // 敵のx座標移動
}

// ゴールまでの距離
void Distance(void)
{
	distance -= speed / 10;
}

// ゴール
void Goal(void)
{
	float space;
	DrawExtendGraph(distance + WIDTH / 2 - 29, HEIGHT - 430, distance + WIDTH / 2 + 319, HEIGHT - 129, imgGoal, true); // ゴール画像の描画
	space = (enemy.x + 184) - (player.x + 158); // プレイヤーと敵の間の距離
	if (player.x + 158 > enemy.x + 184 && distance <= 0) // プレイヤーの勝ち
	{
		distance = 0;
		speed = 0;
		enemy.speed = 0;
		enemySpeed = false;
		sceneTimer++;
		if (sceneTimer > 60)
		{
			scene = 4; // プレイヤー勝ちシーンに遷移
		}
	}
	if (enemy.x + 184 > player.x + 158 && distance <= space) // プレイヤーの負け
	{
		speed = 0;
		enemy.speed = 0;
		enemySpeed = false;
		sceneTimer++;
		if (sceneTimer > 60)
		{
			scene = 5; // プレイヤー負けシーンに遷移
		}
	}
}

// 勝ちシーン
void Win(void)
{
	DrawExtendGraph(WIDTH / 2 - 132, HEIGHT - 140 - 198, WIDTH / 2 + 132, HEIGHT - 140, imgPlayer[2], true);// プレイヤーの画像
	DrawExtendGraph(WIDTH / 2 - 69, HEIGHT - 140 - 198 - 10 - 96, WIDTH / 2 + 69, HEIGHT - 140 - 198 - 10, imgCrown, true); // 王冠画像
	SetFontSize(190);
	DrawFormatString(WIDTH / 2 - 200 + 5, 200 + 5, GetColor(255, 255, 200), "WIN!!"); // win表示
	DrawFormatString(WIDTH / 2 - 200 + 3, 200 + 3, GetColor(255, 200, 0), "WIN!!");
	DrawFormatString(WIDTH / 2 - 200, 200, GetColor(255, 240, 0), "WIN!!");
	if (CheckHitKeyAll()) // キーが入力されたら
	{
		scene = 1; // タイトルへ遷移
	}
}

// 負けシーン
void Lose(void)
{
	DrawExtendGraph(WIDTH / 2 - 121.2, HEIGHT - 333.2, WIDTH / 2 + 121.1, HEIGHT - 140, imgEnemy[2], true); // 敵の画像
	DrawExtendGraph(WIDTH / 2 - 69, HEIGHT - 439.2, WIDTH / 2 + 69, HEIGHT - 343.2, imgCrown, true); // 王冠画像
	SetFontSize(190);
	DrawFormatString(WIDTH / 2 - 195, 205, GetColor(134, 200, 255), "LOSE..."); // lose表示
	DrawFormatString(WIDTH / 2 - 197, 203, GetColor(53, 69, 255), "LOSE...");
	DrawFormatString(WIDTH / 2 - 200, 200, GetColor(44, 110, 255), "LOSE...");
	if (CheckHitKeyAll()) // キーが入力されたら
	{
		scene = 1; // タイトルへ遷移
	}
}

// タイトルシーン
void Title(void)
{
	titleTimer++;
	SetFontSize(220);
	// タイトルの表示
	DrawFormatString(WIDTH / 2 - 404, 220, GetColor(50, 190, 230), "Car Race"); // タイトル（水色）
	DrawFormatString(WIDTH / 2 - 396, 220, GetColor(50, 190, 230), "Car Race");
	DrawFormatString(WIDTH / 2 - 400, 216, GetColor(50, 190, 230), "Car Race");
	DrawFormatString(WIDTH / 2 - 400, 224, GetColor(50, 190, 230), "Car Race");
	DrawFormatString(WIDTH / 2 - 400, 222, GetColor(255, 255, 255), "Car Race"); // タイトル（白）
	DrawFormatString(WIDTH / 2 - 400, 218, GetColor(255, 255, 255), "Car Race");
	DrawFormatString(WIDTH / 2 - 402, 220, GetColor(255, 255, 255), "Car Race");
	DrawFormatString(WIDTH / 2 - 398, 220, GetColor(255, 255, 255), "Car Race");
	DrawFormatString(WIDTH / 2 - 400, 220, GetColor(236, 116, 155), "Car Race"); // タイトル（ピンク）
	if (titleTimer % 60 < 30) // 点滅処理
	{
		SetFontSize(40);
		DrawFormatString(WIDTH / 2 - 170, 470, GetColor(0, 0, 0), "Space To Start"); // スペースキー指示表示（点滅）
	}
	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		scene = 2;
	}
}

// タイトルの車用関数
void TitleCar(void)
{
	carTimer++;
	static int greenX; // スクロールの位置を管理する変数
	static int blueX;
	static int redX;
	static int yellowX;
	int interval = 5; // スピードごとのフレーム間隔
	if (carTimer >= interval) // 一定フレームが経過した
	{
		carTimer = 0;
		if (imageNum == 0) // 画像番号の切り替え
		{
			imageNum = 1;
		}
		else
		{
			imageNum = 0;
		}
	}
	greenX = (greenX + 10) % (WIDTH+WIDTH/2); // 車（green）のX座標
	blueX = (blueX + 10) % (WIDTH+WIDTH/2); // 車（green）のX座標
	redX = (redX + 10) % (WIDTH+WIDTH/2); // 車（green）のX座標
	yellowX = (yellowX + 10) % (WIDTH+WIDTH/2); // 車（green）のX座標
	if (imageNum == 0) // 画像番号でプレイヤーの描画
	{
		DrawGraph(greenX - 100, HEIGHT - 260, imgPlayer[0], true); // 車（green）の描画
		DrawGraph(blueX - 298, HEIGHT - 290, imgBlue[0], true); // 車（blue）の描画
		DrawGraph(redX - 516, HEIGHT - 290, imgRed[0], true); // 車（red）の描画
		DrawGraph(yellowX - 736, HEIGHT - 290, imgEnemy[0], true); // 車（yellow）の描画
	}
	else
	{
		DrawGraph(greenX - 100, HEIGHT - 256, imgPlayer[1], true);
		DrawGraph(blueX - 298, HEIGHT - 286, imgBlue[1], true);
		DrawGraph(redX - 516, HEIGHT - 286, imgRed[1], true);
		DrawGraph(yellowX - 736, HEIGHT - 286, imgEnemy[1], true);
	}
}

// ゲーム説明シーン
void Explanation(void)
{
	// 背景の表示
	static int backEmptyX; // スクロールの位置を管理する変数
	backEmptyX = (backEmptyX - 1) % 1024; // 背景（空）のX座標
	DrawGraph(backEmptyX, -64, imgBackEmpty, false); // 背景（空）の描画
	DrawGraph(backEmptyX + 1024, -64, imgBackEmpty, false);
	DrawGraph(backEmptyX + 2048, -64, imgBackEmpty, false);
	// ゲーム説明
	SetFontSize(40);
	DrawFormatString(WIDTH / 2 - 500, HEIGHT / 2 - 40, GetColor(0, 0, 0), "←,↑,→ を順に押すと速度が上昇！間違えると低下する。\nライバルより先にゴールまで到達しよう！");
	// シーン遷移
	startTimer++;
	if (startTimer > 60)
	{
		countDown--; // カウント減らす
		startTimer = 0;
		if (countDown == 0) // カウントが０になったらシーン遷移
		{
			scene = 3;
		}
	}
	SetFontSize(55);
	DrawFormatString(WIDTH / 2 + 303, HEIGHT - 97, GetColor(255,255,255), "開始まで　%d", countDown); // カウントダウン表示
	DrawFormatString(WIDTH / 2 + 300, HEIGHT - 100, GetColor(0, 0, 0), "開始まで　%d", countDown);
}