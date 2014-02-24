#ifndef SUPERMAN_WELCOME_SCENE_H_
#define SUPERMAN_WELCOME_SCENE_H_

#include "cocos2d.h"

class WelcomeScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
};

class WelcomeLayer : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(Object* pSender);

	void menuNewCallback(Object* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(WelcomeLayer);

	cocos2d::RepeatForever* MyPathFun(float controlX, float controlY, float w);
	cocos2d::ParticleSystem* particleInit();

};

#endif // SUPERMAN_WELCOME_SCENE_H_
