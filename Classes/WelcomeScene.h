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

    virtual bool init();  

    CREATE_FUNC(WelcomeLayer);

	cocos2d::RepeatForever* buildParticleMovePath(float controlX, float controlY, float w, bool isClockwise);

	cocos2d::ParticleSystem* initParticle();

};

#endif // SUPERMAN_WELCOME_SCENE_H_
