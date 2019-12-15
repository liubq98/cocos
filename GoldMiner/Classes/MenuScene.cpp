#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	auto text = Sprite::create("gold-miner-text.png");
	text->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 200));
	this->addChild(text, 0);

	auto biggold = Sprite::create("menu-start-gold.png");
	biggold->setPosition(Vec2(origin.x + visibleSize.width - biggold->getContentSize().width / 2 - 40, origin.y + biggold->getContentSize().width / 2));
	this->addChild(biggold, 0);

	auto start = MenuItemImage::create(
		"start-0.png",
		"start-1.png",
		CC_CALLBACK_1(MenuScene::menuStartCallBack, this));
	start->setPosition(Vec2(origin.x + 150, origin.y + 180));
	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2::ZERO);
	biggold->addChild(menu, 0);

    return true;
}

void MenuScene::menuStartCallBack(Ref* pSender) {
	Director::getInstance()->replaceScene(GameSence::createScene());
}

