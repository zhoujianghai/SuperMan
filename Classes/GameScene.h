#ifndef SUPERMAN_GAME_SCENE_H_
#define SUPERMAN_GAME_SCENE_H_

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
};

class GameLayer : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(Object* pSender);

	void menuNewCallback(Object* pSender);

	void menuSettingCallback(Object* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);
};

#endif // SUPERMAN_GAME_SCENE_H_
