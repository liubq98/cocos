#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Monster.h"
#include "sqlite3.h"

#pragma execution_character_set("utf-8")

using namespace std;

USING_NS_CC;

int flag = 0;
int blood = 100;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	lastDir = "D";

	sqlite3* pdb = NULL;
	int result = sqlite3_open("save.db", &pdb);
	string sql = "create table score(ID int primary key not null, num int);";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	sql = "insert into score values(0,0);";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);


    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 2);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 1);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	// Todo
	dead.reserve(22);
	for (int i = 0; i < 22; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	// Todo
	run.reserve(8);
	for (int i = 0; i < 8; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	auto w = Label::createWithTTF("W", "fonts/arial.ttf", 36);
	auto wItem = MenuItemLabel::create(w, CC_CALLBACK_1(HelloWorld::wEvent, this));
	wItem->setPosition(Vec2(origin.x + visibleSize.width / 2 - 250, origin.y + visibleSize.height / 2 - 100));

	auto s = Label::createWithTTF("S", "fonts/arial.ttf", 36);
	auto sItem = MenuItemLabel::create(s, CC_CALLBACK_1(HelloWorld::sEvent, this));
	sItem->setPosition(Vec2(origin.x + visibleSize.width / 2 - 250, origin.y + visibleSize.height / 2 - 150));

	auto a = Label::createWithTTF("A", "fonts/arial.ttf", 36);
	auto aItem = MenuItemLabel::create(a, CC_CALLBACK_1(HelloWorld::aEvent, this));
	aItem->setPosition(Vec2(origin.x + visibleSize.width / 2 - 300, origin.y + visibleSize.height / 2 - 150));

	auto d = Label::createWithTTF("D", "fonts/arial.ttf", 36);
	auto dItem = MenuItemLabel::create(d, CC_CALLBACK_1(HelloWorld::dEvent, this));
	dItem->setPosition(Vec2(origin.x + visibleSize.width / 2 - 200, origin.y + visibleSize.height / 2 - 150));

	auto direction = Menu::create(wItem, sItem, aItem, dItem, NULL);
	direction->setPosition(Vec2::ZERO);
	this->addChild(direction, 1);

	auto x = Label::createWithTTF("X", "fonts/arial.ttf", 36);
	auto xItem = MenuItemLabel::create(x, CC_CALLBACK_1(HelloWorld::xEvent, this));
	xItem->setPosition(Vec2(origin.x + visibleSize.width / 2 + 300, origin.y + visibleSize.height / 2 - 100));

	auto y = Label::createWithTTF("Y", "fonts/arial.ttf", 36);
	auto yItem = MenuItemLabel::create(y, CC_CALLBACK_1(HelloWorld::yEvent, this));
	yItem->setPosition(Vec2(origin.x + visibleSize.width / 2 + 250, origin.y + visibleSize.height / 2 - 150));

	auto action = Menu::create(xItem, yItem, NULL);
	action->setPosition(Vec2::ZERO);
	this->addChild(action, 1);

	dtime = 180;
	CCString* str_time = CCString::createWithFormat("%d", dtime);
	const char* stime = str_time->getCString();
	time = Label::createWithTTF(stime, "fonts/arial.ttf", 36);
	time->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 150));
	this->addChild(time, 1);

	
	scoreNum = 0;
	CCString* str_score = CCString::createWithFormat("%d", scoreNum);
	const char* score = str_score->getCString();
	scoreLable = Label::createWithTTF(score, "fonts/arial.ttf", 36);
	scoreLable->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 200));
	this->addChild(scoreLable, 1);

	schedule(schedule_selector(HelloWorld::updateCustom), 1.0f, kRepeatForever, 0);


	schedule(schedule_selector(HelloWorld::createMonster), 2.0f, kRepeatForever, 0);

	schedule(schedule_selector(HelloWorld::hitByMonster), 0.1f, kRepeatForever, 0);
	
	schedule(schedule_selector(HelloWorld::stop), 0.1f, kRepeatForever, 0);



	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(tmx, 0);

    return true;
}

void HelloWorld::wEvent(cocos2d::Ref* pSender) {
	if (isDead) 
	{
		return;
	}

	if ((player->getPosition()).y + 50 > visibleSize.height)
	{
		return;
	}
	auto animation = Animation::createWithSpriteFrames(run, 0.1f);
	auto animate = Animate::create(animation);
	auto playerMove = MoveTo::create(0.5, Vec2((player->getPosition()).x, (player->getPosition()).y + 50));
	player->runAction(playerMove);
	player->runAction(animate);

	lastDir = "W";
	
}

void HelloWorld::sEvent(cocos2d::Ref* pSender) {
	if (isDead)
	{
		return;
	}

	if ((player->getPosition()).y - 50 < 0)
	{
		return;
	}
	auto animation = Animation::createWithSpriteFrames(run, 0.1f);
	auto animate = Animate::create(animation);
	auto playerMove = MoveTo::create(0.5, Vec2((player->getPosition()).x, (player->getPosition()).y - 50));
	player->runAction(playerMove);
	player->runAction(animate);

	lastDir = "S";
}

void HelloWorld::aEvent(cocos2d::Ref* pSender) {
	if (isDead)
	{
		return;
	}

	if ((player->getPosition()).x - 50 < 0)
	{
		return;
	}
	auto animation = Animation::createWithSpriteFrames(run, 0.1f);
	auto animate = Animate::create(animation);
	auto playerMove = MoveTo::create(0.5, Vec2((player->getPosition()).x - 50, (player->getPosition()).y));
	player->runAction(playerMove);
	player->runAction(animate);

	if (lastDir != "A")
	{
		player->setFlippedX(true);
	}
	lastDir = "A";
}

void HelloWorld::dEvent(cocos2d::Ref* pSender) {
	if (isDead)
	{
		return;
	}

	if ((player->getPosition()).x + 50 > visibleSize.width)
	{
		return;
	}
	auto animation = Animation::createWithSpriteFrames(run, 0.1f);
	auto animate = Animate::create(animation);
	auto playerMove = MoveTo::create(0.5, Vec2((player->getPosition()).x + 50, (player->getPosition()).y));
	player->runAction(playerMove);
	player->runAction(animate);

	if (lastDir != "D")
	{
		player->setFlippedX(false);
	}
	lastDir = "D";
}

void HelloWorld::xEvent(cocos2d::Ref* pSender) {
	if (isDead)
	{
		return;
	}

	isDead = true;
	if (flag == 1)
	{
		return;
	}
	flag = 1;
	auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
	auto animate = Animate::create(animation);
	auto flagChange = CallFunc::create([this]() {
		flag = 0;
	});
	auto sequence = Sequence::create(animate, flagChange, nullptr);
	player->runAction(sequence);
	unschedule(schedule_selector(HelloWorld::createMonster));
	unschedule(schedule_selector(HelloWorld::hitByMonster));
	unschedule(schedule_selector(HelloWorld::updateCustom));
}

void HelloWorld::yEvent(cocos2d::Ref* pSender) {
	if (isDead)
	{
		return;
	}

	auto fac = Factory::getInstance();
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(), playerRect.getMaxX() - playerRect.getMinX() + 80, playerRect.getMaxY() - playerRect.getMinY());
	Sprite *attackCollision = fac->collider(attackRect);
	if (attackCollision != NULL)
	{
		fac->removeMonster(attackCollision);
		if (pT->getPercentage() < 100)
		{
			auto to = ProgressFromTo::create(1.0f, pT->getPercentage(), pT->getPercentage() + 20);
			pT->runAction(to);
		}

		if (flag == 1)
		{
			return;
		}
		flag = 1;
		auto animation = Animation::createWithSpriteFrames(attack, 0.1f);
		auto animate = Animate::create(animation);
		auto flagChange = CallFunc::create([this]() {
			flag = 0;
		});
		auto sequence = Sequence::create(animate, flagChange, nullptr);
		player->runAction(sequence);

		sqlite3* pdb = NULL;
		scoreNum = scoreNum + 1;
		stringstream ss;
		ss << scoreNum;
		string str_score = ss.str();
		scoreLable->setString(str_score);
		int result = sqlite3_open("save.db", &pdb);
		string sql = "update score set num=" + str_score + " where id=0;";
		int reslut = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	}

}

void HelloWorld::updateCustom(float dt) {
	if (dtime <= 0)
	{
		unschedule(schedule_selector(HelloWorld::update));
		return;
	}

	dtime--;
	CCString* str_time = CCString::createWithFormat("%d", dtime);
	const char* stime = str_time->getCString();
	time->setString(stime);
}

void HelloWorld::updateBloodx(float dt) {
	blood--;
	pT->setPercentage(blood);
}

void HelloWorld::updateBloody(float dt) {
	blood++;
	pT->setPercentage(blood);
}

void HelloWorld::createMonster(float dt) {
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float mx = random(origin.x, visibleSize.width);
	float my = random(origin.y, visibleSize.height);
	m->setPosition(mx, my);
	addChild(m, 3);

	fac->moveMonster(player->getPosition(), 0.5f);
}

void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(player->getBoundingBox());

	if (collision != NULL)
	{
		fac->removeMonster(collision);
		float percentage = pT->getPercentage();
		if (percentage > 0) 
		{
			auto to = ProgressFromTo::create(1.0f, percentage, percentage - 20);
			pT->runAction(to);
		}
		else 
		{
			xEvent(NULL);
		}
	}

	if (pT->getPercentage() <= 0)
	{
		xEvent(NULL);
	}
}

void HelloWorld::stop(float dt)
{
	
}