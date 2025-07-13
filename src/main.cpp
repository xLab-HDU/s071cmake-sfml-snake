#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#define WINDOW_WIDTH 80		//窗口的宽度
#define WINDOW_HEIGHT 25	//窗口的高度
#define STAGE_WIDTH 20		//舞台宽度
#define STAGE_HEIGHT 20		//舞台高度
#define GRIDSIZE  25		//纹理尺寸
#define SCALE     0.5
#define MAXLENGTH 100		//蛇身最大长度
#define INFO_WIDTH 400
#define STEP 0.1

using namespace sf;			//SFML中的每个类都位于该命名空间之下，不设定sf命名空间的话，相应的函数前需要用作用域解析符，例如 sf::VideoMode(width* GRIDSIZE, height* GRIDSIZE)
bool gameOver, gameQuit;
const int width = STAGE_WIDTH;
const int height = STAGE_HEIGHT;
int x, y, fruitX, fruitY, score;
int tailX[MAXLENGTH], tailY[MAXLENGTH];
int nTail;
int delay;
int GameMode;
float stepX, stepY;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir, dir_ing;
void gameOver_info(int _x, int _y);

//这里我们还没有用到类的封装，所以暂时把window作为全局变量。
//对新建窗口的封装，第一个参数是一个VideoMode，表示窗口大小，第二个表示窗口标题.实际上最多可以使用四个参数，最后两个是可选的 -  Style 和ContextSettings。
//标题中有宽字符的，字符串前要加L，不然会显示乱码
sf::RenderWindow window(sf::VideoMode({ width * GRIDSIZE + INFO_WIDTH, height * GRIDSIZE + GRIDSIZE }), L"Snake by 李仕");
Texture tBackground("../data/images/BK.png"), tSnakeHead("../data/images/SH01.png"), tSnakeBody("../data/images/SB0102.png"), tFruit("../data/images/sb0202.png");		//创建4个纹理对象
Sprite spBackground(tBackground), spSnakeHead(tSnakeHead), spSnakeBody(tSnakeBody), spFruit(tFruit);		//创建4个精灵对象
int headRotation;
Font font;
Text text(font);
SoundBuffer sbEat, sbDie;
Sound soundEat(sbEat), soundDie(sbDie);
Music bkMusic;
int soundVolume;		//背景音量
bool MusicOn;			//背景音开关
void Initial()
{
	window.setFramerateLimit(60);	//每秒设置目标帧数

	if (!font.openFromFile("../data/fonts/simsun.ttc"))//选择字体，SFML不能直接访问系统的字体，特殊的字体，需要自己加载
	{
		std::cout << "字体没有找到" << std::endl;
	}
	text.setFont(font);

	// if (!tBackground.loadFromFile("../data/images/BK.png"))//加载纹理图片
	// {
	// 	std::cout << "BK.png 没有找到" << std::endl;
	// }
	// if (!tSnakeHead.loadFromFile("../data/images/SH01.png"))
	// {
	// 	std::cout << "SH01.png 没有找到" << std::endl;
	// }
	// if (!tSnakeBody.loadFromFile("../data/images/SB0102.png"))
	// {
	// 	std::cout << "SB0102.png 没有找到" << std::endl;
	// }
	// if (!tFruit.loadFromFile("../data/images/sb0202.png"))
	// {
	// 	std::cout << "sb0202.png 没有找到" << std::endl;
	// }
	/////////////////////////
	if (!sbEat.loadFromFile("../data/Audios/Eat01.ogg"))//加载音频
	{
		std::cout << "Eat01.ogg 没有找到" << std::endl;
	}
	if (!sbDie.loadFromFile("../data/Audios/Die01.ogg"))//加载音频
	{
		std::cout << "Die01.ogg 没有找到" << std::endl;
	}
	if (!bkMusic.openFromFile("../data/Audios/BGM01.ogg"))//加载背景音乐
	{
		std::cout << "BGM01.ogg 没有找到" << std::endl;
	}
	// spBackground.setTexture(tBackground);					//设置精灵对象的纹理
	// spSnakeHead.setTexture(tSnakeHead);
	// spSnakeBody.setTexture(tSnakeBody);
	// spFruit.setTexture(tFruit);
	spBackground.setOrigin({ GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2 });
	spSnakeHead.setOrigin({ GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2 });
	spSnakeBody.setOrigin({ GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2 });
	spFruit.setOrigin({ GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2 });

	spBackground.setScale({ SCALE, SCALE });
	spSnakeHead.setScale({ SCALE, SCALE });
	spSnakeBody.setScale({ SCALE, SCALE });
	spFruit.setScale({ SCALE, SCALE });

	// soundEat.setBuffer(sbEat);//音效读入缓冲
	// soundDie.setBuffer(sbDie);//音效读入缓冲
	bkMusic.play();				//背景音播放
	bkMusic.setLooping(true);		//背景音循环
	soundVolume = 50;
	MusicOn = true;

	gameOver = false;
	gameQuit = false;
	GameMode = 1;
	stepX = 0.0;
	stepY = 0.0;
	headRotation = 0;
	delay = 0;
	dir = STOP;
	dir_ing = STOP;
	x = width / 2;
	y = height / 2;
	fruitX = rand() % width;
	fruitY = rand() % height;
	score = 0;

	nTail = 1;
	for (int i = 0; i < MAXLENGTH; i++)
	{
		tailX[i] = 0;
		tailY[i] = 0;
	}
}

void Input()
{
	// sf::Event event;//event types 包括Window、Keyboard、Mouse、Joystick，4类消息
	//通过  bool Window :: pollEvent（sf :: Event＆event） 从窗口顺序询问（ polled ）事件。 
	//如果有一个事件等待处理，该函数将返回true，并且事件变量将填充（filled）事件数据。 
	//如果不是，则该函数返回false。 同样重要的是要注意，一次可能有多个事件; 因此我们必须确保捕获每个可能的事件。 
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			window.close();		//窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
			gameOver = false;
			gameQuit = true;
		}

		if (const auto* KeyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			if (KeyReleased->scancode == sf::Keyboard::Scancode::X)
			{
				window.close();		//窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
				gameOver = false;
				gameQuit = true;
			}
			if (KeyReleased->scancode == sf::Keyboard::Scancode::Space)
			{
				if (GameMode == 1)
				{
					GameMode = 2;
					dir_ing = dir;
				}
				else
				{
					GameMode = 1;
					dir_ing = dir;
				}
			}
			if (KeyReleased->code == sf::Keyboard::Key::Add)
			{
				soundVolume += 5;
				bkMusic.setVolume(soundVolume);
			}
			if (KeyReleased->code == sf::Keyboard::Key::Subtract)
			{
				soundVolume -= 5;
				bkMusic.setVolume(soundVolume);
			}
			if (KeyReleased->code == sf::Keyboard::Key::Multiply || KeyReleased->scancode == sf::Keyboard::Scancode::Enter)
			{
				if (MusicOn == true)
				{
					bkMusic.pause(); //bkMusic.stop();
					MusicOn = false;
				}
				else
				{
					bkMusic.play();
					MusicOn = true;
				}
			}
		}

	}


	if (Keyboard::isKeyPressed(Keyboard::Scancode::Left) || Keyboard::isKeyPressed(Keyboard::Scancode::A))	//按键判定
		if (dir != RIGHT)
			dir = LEFT;
	if (Keyboard::isKeyPressed(Keyboard::Scancode::Right) || Keyboard::isKeyPressed(Keyboard::Scancode::D))	//按键判定
		if (dir != LEFT)
			dir = RIGHT;
	if (Keyboard::isKeyPressed(Keyboard::Scancode::Up) || Keyboard::isKeyPressed(Keyboard::Scancode::W))	//按键判定
		if (dir != DOWN)
			dir = UP;
	if (Keyboard::isKeyPressed(Keyboard::Scancode::Down) || Keyboard::isKeyPressed(Keyboard::Scancode::S))	//按键判定
		if (dir != UP)
			dir = DOWN;
}
void Prompt_info(int _x, int _y)
{
	int initialX = 20, initialY = 0;
	int CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold); // |Text::Underlined

	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"■ 游戏说明："); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    A.蛇身自撞，游戏结束"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    B.蛇可穿墙"); window.draw(text);
	initialY += CharacterSize;
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"■ 操作说明："); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 向左移动：←A"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 向右移动：→D"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 向下移动：↓S"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 向上移动：↑W"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 开始游戏：任意方向键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 背景音开关：*或回车"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 背景音音量：+/-键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 动画模式切换：空格键"); window.draw(text);
	initialY += CharacterSize * 1.5;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	if (GameMode == 1)
	{
		text.setFillColor(Color(0, 0, 255, 255));//蓝色字体
		text.setString(L"          步进移动");
	}
	else
	{
		text.setFillColor(Color(255, 0, 0, 255));//红色字体
		text.setString(L"          连续移动");
	}
	window.draw(text);
	text.setFillColor(Color(255, 255, 255, 255));//白色字体
	initialY += CharacterSize * 1.5;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"    □ 退出游戏: x键退出"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"■ 当前得分："); window.draw(text);
	text.setFillColor(Color(255, 0, 0, 255));//红色字体
	//initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX + CharacterSize * 7),static_cast<float>(_y + initialY) });
	CharacterSize = 48;
	text.setCharacterSize(CharacterSize);
	std::stringstream ss;
	ss << score;
	text.setString(ss.str()); window.draw(text);

	CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));//白色字体
	initialY += CharacterSize;
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX),  static_cast<float>(_y + initialY) });
	text.setString(L"■ 作者：杭电数媒 李仕");
	window.draw(text);
}
void Logic()		//上一章的代码，没改
{
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;
	tailX[0] = x;
	tailY[0] = y;

	switch (dir)
	{
	case LEFT:
		x--;
		headRotation = -90;
		break;
	case RIGHT:
		x++;
		headRotation = 90;
		break;
	case UP:
		y--;
		headRotation = 0;
		break;
	case DOWN:
		y++;
		headRotation = 180;
		break;
	default:
		break;
	}
	//if (x > width || x < 0 || y > height || y < 0)
	//	gameOver = true;
	if (x == fruitX && y == fruitY)
	{
		score += 10;
		soundEat.play();//播放吃的音效
		fruitX = rand() % width;
		fruitY = rand() % height;
		nTail++;
	}
	for (int i = 1; i < nTail; i++)
	{
		prev2X = tailX[i];
		prev2Y = tailY[i];
		tailX[i] = prevX;
		tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}

	for (int i = 1; i < nTail; i++)
		if (tailX[i] == x && tailY[i] == y)
		{
			soundDie.play();//播放死亡的音效
			gameOver = true;
		}

	if (x >= width) x = 0;	else if (x < 0)	x = width - 1;
	if (y >= height) y = 0; else if (y < 0) y = height - 1;

	stepX = 0.0;
	stepY = 0.0;
}
void Draw()
{
	window.clear(Color(255, 0, 255, 255));	//清屏
	Prompt_info(width * GRIDSIZE + GRIDSIZE, GRIDSIZE);

	int detaX = GRIDSIZE / SCALE / 2;
	int detaY = GRIDSIZE / SCALE / 2;
	//绘制背景
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			spBackground.setPosition({ static_cast<float>(i * GRIDSIZE + detaX), static_cast<float>(j * GRIDSIZE + detaY) });	//指定纹理的位置
			window.draw(spBackground);							//将纹理绘制到缓冲区
		}
	//绘制蛇
	spSnakeHead.setPosition({ static_cast<float>(tailX[0] * GRIDSIZE + detaX), static_cast<float>(tailY[0] * GRIDSIZE + detaY) });
	spSnakeHead.setRotation(sf::degrees(headRotation));
	window.draw(spSnakeHead);

	for (int i = 1; i < nTail; i++)
	{
		spSnakeBody.setPosition({ static_cast<float>(tailX[i] * GRIDSIZE + detaX), static_cast<float>(tailY[i] * GRIDSIZE + detaY) });
		window.draw(spSnakeBody);
	}
	//绘制水果
	spFruit.setPosition({ static_cast<float>(fruitX * GRIDSIZE + detaX), static_cast<float>(fruitY * GRIDSIZE + detaY) });
	window.draw(spFruit);

	if (gameOver)
		gameOver_info(width / 8 * GRIDSIZE, height / 4 * GRIDSIZE);
	window.display();				//把显示缓冲区的内容，显示在屏幕上。SFML采用的是双缓冲机制
}
void LogicStep() //步进细化
{
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;
	bool updateFlag = false;

	switch (dir_ing)
	{
	case LEFT:
		stepX -= STEP;
		if (stepX < -0.9999 || stepX >= 0.9999)
		{
			x--;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = -90;
			updateFlag = true;
		}
		break;
	case RIGHT:
		stepX += STEP;
		if (stepX < -0.9999 || stepX >= 0.9999)
		{
			x++;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = 90;
			updateFlag = true;
		}
		break;
	case UP:
		stepY -= STEP;
		if (stepY < -0.9999 || stepY >= 0.9999)
		{
			y--;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = 0;
			updateFlag = true;
		}
		break;
	case DOWN:
		stepY += STEP;
		if (stepY < -0.9999 || stepY >= 0.9999)
		{
			y++;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = 180;
			updateFlag = true;
		}
		break;
	default:
		dir_ing = dir;
		break;
	}
	//if (x > width || x < 0 || y > height || y < 0)
	//	gameOver = true;

	if (updateFlag == true)
	{
		if (x == fruitX && y == fruitY)
		{
			soundEat.play();//播放吃的音效
			score += 10;
			fruitX = rand() % width;
			fruitY = rand() % height;
			nTail++;
		}
		for (int i = 1; i < nTail; i++)
		{
			prev2X = tailX[i];
			prev2Y = tailY[i];
			tailX[i] = prevX;
			tailY[i] = prevY;
			prevX = prev2X;
			prevY = prev2Y;
			if (tailX[i] >= width) tailX[i] = 0;	else if (tailX[i] < 0)	tailX[i] = width - 1;
			if (tailY[i] >= height) tailY[i] = 0;  else if (tailY[i] < 0) tailY[i] = height - 1;
		}
		if (x >= width) x = 0;	else if (x < 0)	x = width - 1;
		if (y >= height) y = 0; else if (y < 0) y = height - 1;
		tailX[0] = x;
		tailY[0] = y;
		for (int i = 1; i < nTail; i++)
			if (tailX[i] == x && tailY[i] == y)
			{
				soundDie.play();//播放死亡的音效
				gameOver = true;
			}
	}
}
void DrawStep()
{
	window.clear(Color(255, 0, 255, 255));	//清屏
	Prompt_info(width * GRIDSIZE + GRIDSIZE, GRIDSIZE);

	int detaX = GRIDSIZE / SCALE / 2;
	int detaY = GRIDSIZE / SCALE / 2;
	//绘制背景
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			spBackground.setPosition({ static_cast<float>(i * GRIDSIZE + detaX), static_cast<float>(j * GRIDSIZE + detaY) });	//指定纹理的位置
			window.draw(spBackground);							//将纹理绘制到缓冲区
		}
	//绘制蛇
	float stepLength;
	stepLength = stepX + stepY;
	if (stepLength < 0)
		stepLength = -stepLength;

	spSnakeHead.setPosition({ static_cast<float>((tailX[0] + stepX) * GRIDSIZE + detaX), static_cast<float>((tailY[0] + stepY) * GRIDSIZE + detaY) });
	spSnakeHead.setRotation(sf::degrees(headRotation));
	window.draw(spSnakeHead);

	for (int i = 1; i < nTail; i++)
	{
		if (tailY[i] == tailY[i - 1] && tailX[i] != tailX[i - 1]) //水平跟随
			spSnakeBody.setPosition({ static_cast<float>((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLength) * GRIDSIZE + detaX), static_cast<float>(tailY[i] * GRIDSIZE + detaY) });
		if (tailY[i] != tailY[i - 1] && tailX[i] == tailX[i - 1]) //竖直跟随
			spSnakeBody.setPosition({ static_cast<float>(tailX[i] * GRIDSIZE + detaX), static_cast<float>((tailY[i] + (tailY[i - 1] - tailY[i]) * stepLength) * GRIDSIZE + detaY) });
		if (tailY[i] != tailY[i - 1] && tailX[i] != tailX[i - 1]) //拐角跟随
			spSnakeBody.setPosition({ static_cast<float>((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLength) * GRIDSIZE + detaX), static_cast<float>((tailY[i] + (tailY[i - 1] - tailY[i]) * stepLength) * GRIDSIZE + detaY) });
		window.draw(spSnakeBody);
	}
	// for (int i = 1; i < nTail; i++)
	// {
	// 	if (tailY[i] == tailY[i - 1] && tailX[i] != tailX[i - 1])	//水平跟随
	// 	{
	// 		if (abs(tailX[i - 1] - tailX[i]) == 1)//解决蛇身秒闪过去的动画
	// 		{
	// 			spSnakeBody.setPosition({ static_cast<float>((tailX[i] + (tailX[i - 1] - tailX[i]) * stepLength) * GRIDSIZE + detaX - 2), static_cast<float>(tailY[i] * GRIDSIZE + detaY - 1) });
	// 		}
	// 	}
	// 	if (tailY[i] != tailY[i - 1] && tailX[i] == tailX[i - 1])	//竖直跟随
	// 	{
	// 		if (abs(tailY[i - 1] - tailY[i]) == 1)//解决蛇身秒闪过去的动画
	// 		{
	// 			spSnakeBody.setPosition({ static_cast<float>(tailX[i] * GRIDSIZE + detaX - 2), static_cast<float>((tailY[i] + (tailY[i - 1] - tailY[i]) * stepLength) * GRIDSIZE + detaY - 1 )});
	// 		}
	// 	}
	// 	window.draw(spSnakeBody);
	// }

	//绘制水果
	spFruit.setPosition({ static_cast<float>(fruitX * GRIDSIZE + detaX),static_cast<float> (fruitY * GRIDSIZE + detaY) });
	window.draw(spFruit);

	if (gameOver)
		gameOver_info(width / 8 * GRIDSIZE, height / 4 * GRIDSIZE);

	window.display();				//把显示缓冲区的内容，显示在屏幕上。SFML采用的是双缓冲机制
}
void gameOver_info(int _x, int _y)
{
	int initialX = 20, initialY = 0;
	int CharacterSize = 48;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 0, 0, 255));
	text.setStyle(Text::Bold);

	text.setPosition({ static_cast<float>(_x + initialX), static_cast<float>(_y + initialY) });
	text.setString(L"   游戏结束！！"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition({ static_cast<float>(_x + initialX), static_cast<float>(_y + initialY) });
	text.setString(L" Y重新开始/N退出"); window.draw(text);
}
int main()
{
	do {

		Initial();

		while (window.isOpen() && gameOver == false)
		{
			Input();

			switch (GameMode)
			{
			case 1:
				delay++;
				if (delay % 10 == 0)
				{
					delay = 0;
					Logic();
				}
				Draw();
				break;
			case 2:
				LogicStep();
				DrawStep();
				break;
			}
			
		}
		while (gameOver)
		{
			while (const std::optional event = window.pollEvent()) {
				if (event->is<sf::Event::Closed>()) {
					window.close();
					gameOver = false;
					gameQuit = true;
				}
				if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
					if (keyReleased->scancode == sf::Keyboard::Scancode::Y)
					{
						gameOver = false;
					}
					if (keyReleased->scancode == sf::Keyboard::Scancode::N)
					{
						gameOver = false;
						gameQuit = true;
					}
				}
			}

		}

	} while (!gameQuit);

	//system("pause");
	return 0;
}