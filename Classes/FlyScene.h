#ifndef __FLYSCENE_SCENE_H__
#define __FLYSCENE_SCENE_H__

#include <vector>
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

class FlyScene: public cocos2d::CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();

	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);

	// implement the "static node()" method manually
	CREATE_FUNC(FlyScene);
private:
	void menuReturnCallback(cocos2d::CCObject *pSender);

	void createBullet(float dt);
	void flying(float dt);
	void checkBullet(float dt);
	void saveTime(float dt);
	void explosionEndDid();

	//摇杆相关
	cocos2d::CCSprite *joystick;
	cocos2d::CCPoint centre;
	float radius;

	//飞机相关
	cocos2d::CCSprite *plane;
	float speedX;
	float speedY;
	bool isFlying;	

	//子弹相关
	std::vector<cocos2d::CCSprite*> bullets;

	//爆炸相关
	cocos2d::CCSprite *explosion;
};
#endif  // __FLYSCENE_SCENE_H__