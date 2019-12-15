#include "GameScene.h"

USING_NS_CC;

Scene* GameSence::createScene()
{
	return GameSence::create();
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	auto StoneLayer = Layer::create();
	StoneLayer->setPosition(Vec2(0, 0));
	StoneLayer->setName("StoneLayer");
	this->addChild(StoneLayer, 1);

	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	stone->setName("stone");
	StoneLayer->addChild(stone, 1);

	mouseLayer = Layer::create();
	mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));
	mouseLayer->setName("mouseLayer");
    this->addChild(mouseLayer, 1);

	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate*ani = Animate::create(AnimationCache::getInstance()->getAnimation("eatAnimation"));
	mouse->runAction(RepeatForever::create(ani));
	mouse->setPosition(Vec2(visibleSize.width / 2, 0));
	mouse->setName("mouse");
	mouseLayer->addChild(mouse, 1);

	auto shoot = Label::createWithTTF("shoot", "fonts/arial.ttf", 80);
	shoot->setPosition(Vec2(origin.x + visibleSize.width - shoot->getContentSize().width, 480));
	shoot->setName("shoot");
	this->addChild(shoot, 1);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {

	auto location = touch->getLocation();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto shoot = this->getChildByName("shoot");
	auto StoneLayer = this->getChildByName("StoneLayer");
	auto theStone = StoneLayer->getChildByName("stone");
	auto MouseLayer = this->getChildByName("mouseLayer");
	auto theMouse = MouseLayer->getChildByName("mouse");

	if (shoot->getBoundingBox().containsPoint(location))
	{
		auto stone2 = Sprite::create("stone.png");
		stone2->setPosition(560, 480);
		StoneLayer->addChild(stone2, 1);

		auto stoneMove = MoveTo::create(0.5, MouseLayer->convertToWorldSpace(theMouse->getPosition()));
		auto stoneFade = FadeOut::create(0.5);
		auto removeCallback = CallFunc::create([this, &stone2]() {
			this->removeChild(stone2);
		});
		auto stoneSequence = Sequence::create(stoneMove, stoneFade, removeCallback, nullptr);
		stone2->runAction(stoneSequence);

		auto newPosition = Vec2(CCRANDOM_0_1() * visibleSize.width, CCRANDOM_0_1() * visibleSize.height - visibleSize.height/2);
		auto mouseMove = MoveTo::create(0.5, newPosition);
		theMouse->runAction(mouseMove);

		auto diamond = Sprite::create("diamond.png");
		diamond->setPosition(MouseLayer->convertToWorldSpace(theMouse->getPosition()));
		this->addChild(diamond, 1);
	}
	else
	{
		auto cheese = Sprite::create("cheese.png");
		cheese->setPosition(location);
		this->addChild(cheese,2);

		auto mouseMove = MoveTo::create(0.5, MouseLayer->convertToNodeSpace(location));
		theMouse->runAction(mouseMove);

		auto cheeseFade = FadeOut::create(1);
		auto removeCheese = CallFunc::create([this, &cheese]() {
			this->removeChild(cheese);
		});
		auto cheeseSequence = Sequence::create(DelayTime::create(0.5), cheeseFade, removeCheese, nullptr);
		cheese->runAction(cheeseSequence);
	}

	return true;
}
