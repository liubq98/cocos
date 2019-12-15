#pragma once
#include <stdio.h>
#include "cocos2d.h"

class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

	void MenuScene::menuStartCallBack(Ref* pSender);

    virtual bool init();
    
	// a selector callback
	//void startMenuCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MenuScene);
};
