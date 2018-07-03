//---------------------------------------------------------
//	Kikyu!
//
//		©2013 Yuichiro Nakada
//---------------------------------------------------------
//g++ kikyu.cpp -o kikyu -I ../SoftPixelEngine/sources/ -L. -lSoftPixelEngine
//g++ kikyu.cpp -o kikyu -I ../softpixelengine-code/sources/ -L. -lSoftPixelEngine -lpthread -lopenal
//LD_LIBRARY_PATH=. ./kikyu

#include <SoftPixelEngine.hpp>
#include "font.hpp"
using namespace sp;

// パラメータ
#define SCREEN_WIDTH		800
#define SCREEN_HEIGHT		600
// プレイヤー
#define PLAYER_WIDTH		48	// 幅
#define PLAYER_HEIGHT		64	// 高さ
#define PLAYER_JUMP		-5	// ジャンプの強さ
#define PLAYER_GRAVIRY		0.25	// 重力
// 敵
#define ENEMY_WIDTH		32	// 幅
#define ENEMY_HEIGHT		32	// 高さ
#define ENEMY_SPEED		-5	// 移動スピード
#define ENEMY_HIT_LENGTH	20	// 衝突判定の領域サイズ
#define ENEMY_MAX		20	// 最大出現数
// アイテム
#define ITEM_WIDTH		16	// 幅
#define ITEM_HEIGHT		16	// 高さ
#define ITEM_SPEED		-4	// アイテムのスピード
#define ITEM_MAX		20	// 最大出現数
#define COIN_POINT		100	// コインのポイント
#define COIN_FRAME		14	// コイン画像のフレームインデックス
#define DIAMOND_POINT		1000	// ダイアモンドのポイント
#define DIAMOND_FRAME		64	// ダイアモンドのフレームインデックス
// 背景
#define BACKGROUND_WIDTH	1320
#define BACKGROUND_HEIGHT	320
// 画像
#if defined(__ANDROID__)
#define RESOURCE_PATH		"./"
#else
#define RESOURCE_PATH		"./assets/"
#endif
#define IMAGE_PLAYER		RESOURCE_PATH "Balloon.png"	// http://korcs.info/material.html
#define IMAGE_ENEMY		RESOURCE_PATH "enemy.png"
#define IMAGE_ICON		RESOURCE_PATH "icon0.png"
#define IMAGE_BACKGROUND	RESOURCE_PATH "side01.jpg"
// 音
#define MAIN_BGM		RESOURCE_PATH "main_bgm.wav"
#define JUMP_SE			RESOURCE_PATH "jump.wav"
#define ITEM_GET_SE		RESOURCE_PATH "itemget.wav"

/* === Global members === */
SoftPixelDevice* spDevice	= 0;
io::InputControl* spControl	= 0;
video::RenderSystem* spRenderer	= 0;
video::RenderContext* spContext	= 0;
audio::SoundDevice* spListener	= 0;
audio::Sound* se[3];
FontTex *font;

int frame;	// フレーム
int score;	// スコア
struct Sprites {
	int frame;
	float x, y;
	float vx, vy;		// 移動速度
	int time;		// 存在時間
	int jumpPow;		// ジャンプ力
	int jumpAble;		// ジャンプ可
};
class Sprite {
public:
	Sprites *s;
	int num;

//	int frame;
	int cx, cy;		// チップの大きさ
	int width, height;	// 表示サイズ
/*	float x, y;
	float vx, vy;		// 移動速度
	int time;		// 存在時間
	int jumpPow;		// ジャンプ力
	int jumpAble;		// ジャンプ可*/
	video::Texture *tex;	// テクスチャー

	Sprite(int n=1) {
		num = n;
		s = new Sprites[num];
	}
	~Sprite() {
		delete s;
	}

	void draw(int n=0) {
		float w = s[n].frame % (tex->getSize().Width / cx);
		float h = s[n].frame / (tex->getSize().Width / cy);
		spRenderer->draw2DImage(
			tex,
			dim::rect2di(s[n].x, s[n].y, width, height),
			dim::rect2df(
				w * cx / tex->getSize().Width,
				h * cy / tex->getSize().Height,
				(w+1) * cx / tex->getSize().Width,
				(h+1) * cy / tex->getSize().Height
			)
		);
	}

	bool intersect(Sprite *a, int n=0) {
		if (s[n].x <= a->s[0].x+a->width && s[n].y <= a->s[0].y+a->height
			&& s[n].x+width >= a->s[0].x && s[n].y+height >= a->s[0].y) {
			return true;
		} else {
			return false;
		}
	}
};
Sprite player;			// プレイヤー
Sprite enemy(ENEMY_MAX);	// 敵
Sprite item(ITEM_MAX);		// アイテム
Sprite background;		// 背景


#if defined(__ANDROID__)
void InitDevice(android_app* App)
#else
void InitDevice()
#endif
{
	// Create the graphics device
#if defined(__ANDROID__)
	spDevice   = createGraphicsDevice(
		App, video::RENDERER_OPENGLES1, "Kikyu!"
	);
#else
	spDevice    = createGraphicsDevice(
		video::RENDERER_AUTODETECT, dim::size2di(SCREEN_WIDTH, SCREEN_HEIGHT), 32, "Kikyu!"
	);
#endif

	// Create input control and get render system
	spControl   = spDevice->getInputControl();
	spRenderer  = spDevice->getRenderSystem();
	spContext   = spDevice->getRenderContext();
	spListener  = spDevice->getSoundDevice();

	// Make the background black
	spRenderer->setClearColor(0);

	// Update window title
	spDevice->setWindowTitle(
		spDevice->getWindowTitle() + " [ " + spRenderer->getVersion() + " ]"
	);

	// To be sure the programs runs on each PC with the same speed activate the frame rates
	spDevice->setFrameRate(100);
}

void LoadResources()
{
	// for fonts
	font = new FontTex(spRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, RESOURCE_PATH);

	// Load background texture
	background.tex = spRenderer->loadTexture(IMAGE_BACKGROUND);
	background.tex->setWrapMode(video::TEXWRAP_CLAMP);

	// Load character texture
	player.tex = spRenderer->loadTexture(IMAGE_PLAYER);
	enemy.tex = spRenderer->loadTexture(IMAGE_ENEMY);
	item.tex = spRenderer->loadTexture(IMAGE_ICON);

	// Load sounds
	se[0] = spListener->loadSound(MAIN_BGM);
	se[1] = spListener->loadSound(JUMP_SE);
	se[2] = spListener->loadSound(ITEM_GET_SE);
}

void Setting()
{
	// スコア加算
	score = 0;

	// 背景
	background.s->frame = 0;
	background.s->x = 0;
	background.s->y = 0;
	background.s->vx = (float)SCREEN_WIDTH / background.tex->getSize().Width;

	// プレイヤー
	player.s->frame = 0;
	player.s->x = 20;
	player.s->y = SCREEN_HEIGHT/2-PLAYER_HEIGHT/2;
	player.s->vy = 0;
	player.width = player.cx = PLAYER_WIDTH;
	player.height = player.cy = PLAYER_HEIGHT;

	// 敵
	enemy.width = enemy.cx = ENEMY_WIDTH;
	enemy.height = enemy.cy = ENEMY_HEIGHT;
	for (int i=0; i<ENEMY_MAX; i++) {
		enemy.s[i].x = -ENEMY_WIDTH;
	}

	// アイテム
	item.width = item.cx = ITEM_WIDTH;
	item.height = item.cy = ITEM_HEIGHT;
	for (int i=0; i<ITEM_MAX; i++) {
		item.s[i].x = -ITEM_WIDTH;
	}
}

void (*Scene)();
void GameOver();
void DrawScene()
{
	int i;

	// スコア加算
	score++;

	// 背景描画
	spRenderer->draw2DImage(
		background.tex,
		dim::rect2di(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
		dim::rect2df(background.s->x, background.s->y, background.s->vx, 1)
	);
	// スクロール
	background.s->x += 4.0/background.tex->getSize().Width;
	background.s->vx += 4.0/background.tex->getSize().Width;
	// 端まで行ったら戻す
	if (background.s->vx >= 1 /*+ (float)SCREEN_WIDTH / background.tex->getSize().Width*/) {
		background.s->vx = (float)SCREEN_WIDTH / background.tex->getSize().Width;
		background.s->x = 0;
		/*background.s->vx = 0;
		background.s->x = -(float)SCREEN_WIDTH / background.tex->getSize().Width;*/
	}

	// プレイヤー処理
	player.s->vy += PLAYER_GRAVIRY;		// 重力
	player.s->y  += player.s->vy;		// 移動

	// フレームアニメーション
	/*if (player.vy > 0) player.frame = 0;
	else player.frame = 1;
	player.frame += (frame/4 % 12)*2;*/
	player.s->frame = frame/4 % 24;
	//printf("%d\n", player.frame);
	//DrawSprite(player.tex, player.x, player.y, player.frame);
	player.draw();

	// 画面からはみ出た際はゲームオーバー
	int top     = 0;
	int bottom  = SCREEN_HEIGHT;
	if (player.s->y+player.height < top) {
		Scene = GameOver;
	} else if (player.s->y > bottom) {
		Scene = GameOver;
	}

	// ジャンプ
#if defined(__ANDROID__)
	if (spControl->getPointerCount() == 1)
#else
	if (spControl->keyHit(io::KEY_SPACE))
#endif
	{
		player.s->vy = PLAYER_JUMP;
		// SE 再生
		se[1]->play();
	}

	// アイテムを生成
	if (frame % 50 == 0) {
		for (i=0; i<ITEM_MAX; i++) if (item.s[i].x < -ITEM_WIDTH) break;
		if (i<ITEM_MAX) {
			int r = rand() % 100;
			if (r < 10) {
				item.s[i].frame = DIAMOND_FRAME;
			} else {
				item.s[i].frame = COIN_FRAME;
			}
			item.s[i].x = SCREEN_WIDTH+30;
			item.s[i].y = rand() % (SCREEN_HEIGHT-ITEM_HEIGHT);
		}
	}

	for (i=0; i<ITEM_MAX; i++) {
		if (item.s[i].x < -ITEM_WIDTH) continue;

		// 移動
		item.s[i].x += ITEM_SPEED;
		//DrawSprite(item.tex, item.s[i].x, item.s[i].y, item.s[i].frame);
		item.draw(i);

		// 衝突判定
		if (item.intersect(&player, i)) {
			// 削除
			item.s[i].x = -ITEM_WIDTH-1;
			if (item.s[i].frame == COIN_FRAME) {
				// スコア加算
				score += COIN_POINT;
			} else {
				// スコア加算
				score += DIAMOND_POINT;
			}
			// SE 再生
			se[2]->play();
		}
	}

	// 敵を生成
	if (frame % 30 == 0) {
		for (i=0; i<ENEMY_MAX; i++) if (enemy.s[i].x < -ENEMY_WIDTH) break;
		if (i<ENEMY_MAX) {
			enemy.s[i].frame = enemy.s[i].time = 0;
			enemy.s[i].x = SCREEN_WIDTH+30;
			enemy.s[i].y = rand() % (SCREEN_HEIGHT-ENEMY_HEIGHT);
		}
	}

	for (i=0; i<ENEMY_MAX; i++) {
		if (enemy.s[i].x < -ENEMY_WIDTH) continue;

		// 移動
		enemy.s[i].x += ENEMY_SPEED;
		enemy.s[i].y += cos(enemy.s[i].time*5*3.14/180);

		// フレームアニメーション
		if (enemy.s[i].time % 5 == 0) {
			enemy.s[i].frame += 1;
			enemy.s[i].frame %= 3;
		}
		//DrawSprite(enemy.tex, enemy.s[i].x, enemy.s[i].y, enemy.s[i].frame);
		enemy.draw(i);

		// プレイヤーとの衝突判定
		if (enemy.intersect(&player, i)) {
			Scene = GameOver;
		}
		/*if (this.within(player, ENEMY_HIT_LENGTH)) {
			gameOver("鳥と衝突してしまいました. 残念!!");
		}*/

		// タイムを進める
		enemy.s[i].time++;
	}

	// スコア表示
	char msg[256];
	sprintf(msg, "SCORE: %d", score);
	font->DrawString(0, 0, msg);
}
void Title()
{
	font->DrawStringCenter(SCREEN_HEIGHT/2-64, (char*)"Kikyu!");
	font->DrawPString(100, 100, (char*)"キ♥Aeronaut!");
	font->DrawPString(SCREEN_WIDTH/2-120, SCREEN_HEIGHT/2+32, (char*)"Press Return to Embark");
#if defined(__ANDROID__)
	if (spControl->getPointerCount() == 1)
#else
	if (spControl->keyHit(io::KEY_RETURN))
#endif
	{
		Setting();
		frame = 0;
		Scene = DrawScene;
		se[0]->play();
	}
}
void GameOver()
{
	se[0]->stop();

	font->DrawStringCenter(SCREEN_HEIGHT/2, (char*)"Game Over");
	char msg[256];
	sprintf(msg, "SCORE: %d", score);
	font->DrawString(0, 0, msg);
#if defined(__ANDROID__)
	if (spControl->getPointerCount() == 1)
#else
	if (spControl->keyHit(io::KEY_RETURN))
#endif
	{
		frame = 0;
		Scene = Title;
	}
}

#if defined(__ANDROID__)
void android_main(android_app* App)
#else
int main()
#endif
{
#if defined(__ANDROID__)
	InitDevice(App);
#else
	InitDevice();
#endif
	LoadResources();
	Scene = Title;

	// Loop until the user presses the ESC key
#if defined(__ANDROID__)
	while (spDevice->updateEvent())
#else
	while (spDevice->updateEvent() && !spControl->keyDown(io::KEY_ESCAPE))
#endif
	{
		spRenderer->clearBuffers();

		spRenderer->beginDrawing2D();
		{
			//DrawScene();
			Scene();
			frame++;
		}
		spRenderer->endDrawing2D();

		spContext->flipBuffers();
	}

	// Delete all allocated memor (objects, textures etc.) and close the screen
	deleteDevice();

#if defined(__ANDROID__)
	return;
#else
	return 0;
#endif
}
