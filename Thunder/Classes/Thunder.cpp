#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

// add layer as a child to scene
scene->addChild(layer);

// return the scene
return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	// Todo
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("music/explore.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("music/fire.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	// Todo
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	// Todo
	for (auto i : enemys)
	{
		auto y = (i->getPosition()).y;
		auto x = (i->getPosition()).x;
		i->setPosition(Vec2(x, y - 50));
	}

	int n = 5;
	for (unsigned i = 0; i < 1; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - stoneType % 3);
		stoneType++;
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 0.5), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 移动飞船
void Thunder::movePlane(char c) {
	// Todo
	if (c == 'A')
	{
		if ((player->getPosition()).x - player->getContentSize().width - 10 < 0)
		{
			return;
		}
		auto planeMove = MoveTo::create(0.5, Vec2((player->getPosition()).x - 10, (player->getPosition()).y));
		player->runAction(planeMove);
	}
	else if(c == 'D')
	{
		if ((player->getPosition()).x + player->getContentSize().width + 10 > visibleSize.width)
		{
			return;
		}
		auto planeMove = MoveTo::create(0.5, Vec2((player->getPosition()).x + 10, (player->getPosition()).y));
		player->runAction(planeMove);
	}
}

//发射子弹
void Thunder::fire() {
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

	// 移除飞出屏幕外的子弹
	// Todo
	auto bulletMove = MoveTo::create(1, Vec2((player->getPosition()).x, (player->getPosition()).y + visibleSize.height + 20));
	bullet->runAction(bulletMove);
}

// 切割爆炸动画帧
void Thunder::explosion() {
	// Todo
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	explore.reserve(8);
	for (int i = 0; i < 5; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(193 * i + 1, 0, 142, 142)));
		explore.pushBack(frame);
	}
	for (int i = 0; i < 3; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(193 * i + 1, 193, 142, 142)));
		explore.pushBack(frame);
	}
}

void Thunder::update(float f) {
	for (auto i : enemys)
	{
		if ((i->getPosition()).y <= (player->getPosition()).y || i->getPosition().getDistance(player->getPosition()) < 50)
		{
			stopAc();
		}
	}

	for (auto i : bullets)
	{
		if (i->getPosition().y == (player->getPosition()).y + visibleSize.height + 20)
		{
			auto removeBullet = CallFunc::create([i, this]() {
				bullets.remove(i);
				i->removeFromParentAndCleanup(true);
			});
			i->runAction(removeBullet);
		}
	}

	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	// 飞船移动
	if (isMove)
	{
		this->movePlane(movekey);
	}
		

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemys) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);

}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	// 判断子弹是否打中陨石并执行对应操作
	// Todo
	for (auto i : bullets)
	{
		Rect bulletRect = i->getBoundingBox();
		for (auto j : enemys)
		{
			if (bulletRect.containsPoint(j->getPosition()))
			{
				auto removeBullet = CallFunc::create([i, this]() {
					i->removeFromParentAndCleanup(true);
				});
				i->runAction(removeBullet);

				SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);

				auto animation = Animation::createWithSpriteFrames(explore, 0.05f, 1);
				auto animate = Animate::create(animation);
				auto removeEnemy = CallFunc::create([j, this]() {
					j->removeFromParentAndCleanup(true);
				});
				auto sequence = Sequence::create(animate, removeEnemy, nullptr);
				j->runAction(sequence);

				bullets.remove(i);
				enemys.remove(j);
				return;
			}
		}
	}
}






void Thunder::stopAc() {
    // Todo
	unschedule(schedule_selector(Thunder::update));

	_eventDispatcher->removeAllEventListeners();

	auto gg = Sprite::create("gameOver.png");
	gg->setAnchorPoint(Vec2(0.5, 0.5));
	gg->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(gg, 1);

	auto animation = Animation::createWithSpriteFrames(explore, 0.1f);
	auto animate = Animate::create(animation);
	auto removePlayer = CallFunc::create([this]() {
		player->removeFromParentAndCleanup(true);
	});
	auto sequence = Sequence::create(animate, removePlayer, nullptr);
	player->runAction(sequence);
}



// 添加自定义监听器
void Thunder::addCustomListener() {
	// Todo
	auto customListener = EventListenerCustom::create("meet",CC_CALLBACK_1(Thunder::meet, this));
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(customListener, this);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	// Todo
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	// Todo
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30)
        isClick = true;
    // Todo
	fire();
	return isClick;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	// Todo
	if (isClick == false)
	{
		return;
	}
	Vec2 delta = touch->getDelta();
	player->setPosition(delta.x + (player->getPosition()).x, (player->getPosition()).y);
}
