#ifndef __FLYSCENE_SCENE_H__
#define __FLYSCENE_SCENE_H__

#include <vector>
#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"


typedef enum
{
	SCREEN_DIR_UP = 0,
	SCREEN_DIR_DOWN,
	SCREEN_DIR_LEFT,
	SCREEN_DIR_RIGHT
}SCREEN_DIR;

typedef enum
{
	SPRITE_PLANE = 1,
	SPRITE_BULLET
}SPRITE_TAG;

class Plane;
class MyContactListener;
class FlyScene: public cocos2d::Layer
{
public:
	FlyScene();
	virtual ~FlyScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	void initPhysics();

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::Scene* scene();

	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);

	void pausePressed(cocos2d::Object *pSender);

	void addBoxBodyForSprite(cocos2d::Sprite *sprite);

	// implement the "static node()" method manually
	CREATE_FUNC(FlyScene);

private:
	void menuReturnCallback(cocos2d::Object *pSender);

	void createBullet(float dt);
	void updateBullet(float dt);
	void updatePlane(float dt);
	void updateScore(float dt);
	void updateBoxBody(float dt);

	void explosionEndDid();

	void showJoystick(cocos2d::Point pos);
	void hideJoystick();
	void updateJoystick(cocos2d::Point direction, float distance);

	Plane *_plane;
	bool isFlying;	

	cocos2d::Array *_bullets;
	cocos2d::SpriteBatchNode* _sprite_batch;

	cocos2d::Sprite *_explosion;
	cocos2d::LabelAtlas *_scoreLabel;

	b2World *_world;
	MyContactListener *_contactListener;

	bool _isGameOver;

	cocos2d::Sprite *m_pJoystick;
	cocos2d::Sprite *m_pJoystickBg;
};

class GameOverLayer : public cocos2d::LayerColor
{
public:
	GameOverLayer();
	virtual ~GameOverLayer();

	bool init();

	void menuNewCallback(Object* pSender);

	CREATE_FUNC(GameOverLayer);
};

#endif  // __FLYSCENE_SCENE_H__