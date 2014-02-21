#ifndef __FLYSCENE_SCENE_H__
#define __FLYSCENE_SCENE_H__

#include <vector>
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

typedef enum
{
	SCREEN_DIR_UP = 0,
	SCREEN_DIR_DOWN,
	SCREEN_DIR_LEFT,
	SCREEN_DIR_RIGHT
}SCREEN_DIR;

class Plane;
class FlyScene: public cocos2d::Layer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::Scene* scene();

	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);

	void pausePressed(cocos2d::Object *pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(FlyScene);
private:
	void menuReturnCallback(cocos2d::Object *pSender);

	void createBullet(float dt);
	void checkBullet(float dt);
	void saveTime(float dt);
	void explosionEndDid();


	//飞机相关
	Plane *_plane;
	bool isFlying;	

	//子弹相关
	cocos2d::Array *_bullets;
	cocos2d::SpriteBatchNode* _sprite_batch;

	//爆炸相关
	cocos2d::Sprite *_explosion;
	cocos2d::LabelAtlas *_scoreLabel;
};
#endif  // __FLYSCENE_SCENE_H__