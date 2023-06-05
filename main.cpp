#include "MyGame.h"

// Windowsアプリでのエントリーポイント (main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ゲームインスタンス生成
	MyGame game;

	// ゲームの初期化
	game.Initialize();

	// ゲームループ
	while (true)
	{
		// 更新処理
		game.Update();

		// 終了フラグでブレイク
		if (game.IsEndRequest() == true)
		{
			break;
		}

		// 描画処理
		game.Draw();
	}

	// ゲームの終了
	game.Finalize();

	return 0;
}