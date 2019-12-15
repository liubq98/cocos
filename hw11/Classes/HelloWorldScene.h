#pragma once
#include "cocos2d.h"
#include "sqlite3.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void wEvent(cocos2d::Ref* pSender);
	void sEvent(cocos2d::Ref* pSender);
	void aEvent(cocos2d::Ref* pSender);
	void dEvent(cocos2d::Ref* pSender);
	void xEvent(cocos2d::Ref* pSender);
	void yEvent(cocos2d::Ref* pSender);

	void updateCustom(float dt);

	void updateBloodx(float dt);
	void updateBloody(float dt);

	void hitByMonster(float dt);

	void createMonster(float dt);

	void stop(float dt);
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* scoreLable;
	int dtime;
	cocos2d::ProgressTimer* pT;

	bool isDead;
	int scoreNum;
	std::string lastDir;
};
