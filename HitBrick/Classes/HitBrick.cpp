#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug 模式
  // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();


  auto edgeSp = Sprite::create();  //创建一个精灵
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
  //boundBody->setTag(0);
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);


  preloadMusic(); // 预加载音效

  addSprite();    // 添加背景和各种精灵
  addListener();  // 添加监听器 
  addPlayer();    // 添加球与板
  BrickGeneraetd();  // 生成砖块


  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;

  spFactor = 0;
  return true;
}

// 关节连接，固定球与板子
// Todo
void HitBrick::setJoint() {
	joint1 = PhysicsJointPin::construct(player->getPhysicsBody(), ball->getPhysicsBody(), player->getAnchorPoint(), Vec2(ball->getAnchorPoint().x, ball->getAnchorPoint().y - 35));
	m_world->addJoint(joint1);
}



// 预加载音效
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);


  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 0);
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
  shipbody->setCategoryBitmask(0xFFFFFFFF);
  shipbody->setCollisionBitmask(0xFFFFFFFF);
  shipbody->setContactTestBitmask(0xFFFFFFFF);
  shipbody->setTag(1);
  shipbody->setDynamic(false);  // ??????岻?????????, ????????????б??
  ship->setPhysicsBody(shipbody);
  this->addChild(ship, 1);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);
}

// 添加监听器
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {



  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  // 设置板的刚体属性
  // Todo
  auto playerbody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(100.0f, 1.0f, 0.0f));
  playerbody->setCategoryBitmask(0xFFFFFFFF);
  playerbody->setCollisionBitmask(0xFFFFFFFF);
  playerbody->setContactTestBitmask(0xFFFFFFFF);
  playerbody->setTag(2);
  playerbody->setDynamic(false);
  player->setPhysicsBody(playerbody);


  this->addChild(player, 2);
  
  ball = Sprite::create("ball.png");
  ball->setPosition(Vec2(xpos, player->getPosition().y + ball->getContentSize().height*0.1f));
  ball->setScale(0.1f, 0.1f);
  // 设置球的刚体属性
  // Todo
  auto ballbody = PhysicsBody::createCircle(ball->getContentSize().width / 2, PhysicsMaterial(100.0f, 1.0f, 0.0f));
  ballbody->setCategoryBitmask(0xFFFFFFFF);
  ballbody->setCollisionBitmask(0xFFFFFFFF);
  ballbody->setContactTestBitmask(0xFFFFFFFF);
  ballbody->setTag(3);
  ballbody->setVelocity(Vec2(0, 0));
  ballbody->setGravityEnable(false);
  ballbody->setRotationEnable(false);
  ballbody->setDynamic(true);
  ball->setPhysicsBody(ballbody);

  fireworks = ParticleFireworks::create();
  fireworks->setPosition(ball->getPosition());
  addChild(fireworks);

  addChild(ball, 3);
  
}

// 实现简单的蓄力效果
// Todo
void HitBrick::update(float dt) {
	if (isMove)
	{
		this->movePlayer(moveKey);
	}
	else
	{
		player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	}

	if (spHolded)
	{
		this->addFactor(5);
	}
	fireworks->setPosition(ball->getPosition());
}

void HitBrick::movePlayer(char c) {
	// Todo
	if (c == 'A')
	{
		if (player->getBoundingBox().getMinX() < 0)
		{
			return;
		}

		player->getPhysicsBody()->setVelocity(Vec2(-300, 0));
	}
	else if (c == 'D')
	{
		if (player->getBoundingBox().getMaxX() > visibleSize.width)
		{
			return;
		}

		player->getPhysicsBody()->setVelocity(Vec2(300, 0));
	}
}

void HitBrick::addFactor(int num) {
	spFactor = spFactor + num;
}


// Todo
void HitBrick::BrickGeneraetd() {

for (int i = 0; i < 3; i++) {
	int cw = 0;
	while (cw <= visibleSize.width) {
		auto box = Sprite::create("box.png");
		box->setPosition(cw+0.5*box->getContentSize().width,visibleSize.height-i*box->getContentSize().height - 0.5*box->getContentSize().height);
		// 为砖块设置刚体属性
		// Todo
		auto boxbody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(100.0f, 1.0f, 0.0f));
		boxbody->setCategoryBitmask(0xFFFFFFFF);
		boxbody->setCollisionBitmask(0xFFFFFFFF);
		boxbody->setContactTestBitmask(0xFFFFFFFF);
		boxbody->setTag(10);
		boxbody->setDynamic(false);
		box->setPhysicsBody(boxbody);
		addChild(box, 3);

		cw = cw + box->getContentSize().width;
	}

 }

}


// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	  moveKey = 'A';
	  isMove = true;
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // 左右移动
    // Todo
	  moveKey = 'D';
	  isMove = true;
    break;

  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // 开始蓄力
	  spHolded = true;
      break;
  default:
    break;
  }
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // 停止运动
    // Todo
	  isMove = false;
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // 蓄力结束，小球发射
	  spHolded = false;
	  if (onBall == true)
	  {
		  onBall = false;
		  ball->getPhysicsBody()->setVelocity(Vec2(0, spFactor));
		  m_world->removeJoint(joint1);
	  }
    break;

  default:
    break;
  }
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
  auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
  auto node1 = (Sprite*)c1->getBody()->getNode();
  auto node2 = (Sprite*)c2->getBody()->getNode();

  auto tag1 = node1->getPhysicsBody()->getTag();
  auto tag2 = node2->getPhysicsBody()->getTag();

  if (tag1 == 3 && tag2 == 1)
  {
	  GameOver();
  }
  else if (tag1 == 3 && tag2 == 1)
  {
	  GameOver();
  }
  else if (tag1 == 10 && tag2 == 3)
  {
	  node1->removeFromParent();
  }
  else if (tag2 == 10 && tag1 == 3)
  {
	  node2->removeFromParent();
  }

  return true;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
