#ifndef SUPERMAN_GAMESCENE_SCENE_H_
#define SUPERMAN_GAMESCENE_SCENE_H_

#include <vector>
#include "Box2D/Box2D.h"
#include "cocos2d.h"

//#include "GLES-Render.h"

typedef enum
{
	SPRITE_PLANE = 1,
	SPRITE_BULLET
}SPRITE_TAG;

class Plane;
class MyContactListener;
class GameScene: public cocos2d::Layer
{
public:
	GameScene();
	virtual ~GameScene();

	virtual bool init();  

	static cocos2d::Scene* scene();

	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);

	//void draw();
	void explosionEndDid();

	CREATE_FUNC(GameScene);

private:
	void createBullet(float dt);
	void updateBullet(float dt);
	void updateScore(float dt);

	void initPhysics();
	void addBoxBodyForSprite(cocos2d::Sprite *sprite);
	void updateBoxBody(float dt);


private:
	Plane *_plane;
	bool _isFlying;
	cocos2d::Size _screenSize;

	cocos2d::Array *_bullets;
	cocos2d::SpriteBatchNode* _spriteBatch;
	cocos2d::Rect _screenRect;

	b2World *_world;
	MyContactListener *_contactListener;
	//GLESDebugDraw *_debugDraw;

	cocos2d::Sprite *_explosion;
	cocos2d::LabelAtlas *_scoreLabel;

	bool _isGameOver;
};


class GameOverLayer : public cocos2d::LayerColor
{
public:
	GameOverLayer();
	virtual ~GameOverLayer();

	bool init();

	CREATE_FUNC(GameOverLayer);
};

#endif  // SUPERMAN_GAMESCENE_SCENE_H_