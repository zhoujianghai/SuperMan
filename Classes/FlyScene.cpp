#include "FlyScene.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "global.h"
#include "tools.h"

using namespace cocos2d;
using namespace std;

bool FlyScene::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(! Layer::init());

		Size s = Director::sharedDirector()->getWinSize();	

		//返回
		MenuItemFont::setFontSize(22);
		MenuItemFont::setFontName("American Typewriter");

		auto systemMenu = MenuItemFont::create("Back", CC_CALLBACK_1(FlyScene::menuReturnCallback, this));
		auto mn = Menu::create(systemMenu, NULL);
		mn->setPosition(ccp(0,0));
		systemMenu->setAnchorPoint(ccp(1, 0));
		systemMenu->setPosition(ccp(s.width, 0));
		this->addChild(mn,1);

		//飞机
		auto textureCache = Director::getInstance()->getTextureCache();
		auto planeTexture = textureCache->addImage("plane.png");
		_plane = Sprite::createWithTexture(planeTexture);
		_plane->setPosition(ccp(s.width/2, s.height/2));
		this->addChild(_plane, 0);

		//开启触摸
		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(FlyScene::onTouchesBegan, this);
		listener->onTouchesMoved = CC_CALLBACK_2(FlyScene::onTouchesMoved, this);
		listener->onTouchesEnded = CC_CALLBACK_2(FlyScene::onTouchesEnded, this); 
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		//开启逻辑
		this->schedule(schedule_selector(FlyScene::createBullet));
		this->schedule(schedule_selector(FlyScene::checkBullet));
		this->schedule(SEL_SCHEDULE(&FlyScene::saveTime),1);

		//清零数据
		isFlying = false;
		g_gameTime = 0;
		_bullets = Array::create();
		_bullets->retain();

		bRet = true;
	}while(0);

	return bRet;
}

cocos2d::Scene* FlyScene::scene()
{
	Scene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = Scene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		FlyScene *layer = FlyScene::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

void FlyScene::menuReturnCallback( Object *pSender )
{
	auto scene = GameScene::createScene();
	Director::sharedDirector()->replaceScene(TransitionFlipX::create(1.2f, scene));
}

void FlyScene::onTouchesBegan(const vector<Touch*>& touches, Event *unused_event)
{
	vector<Touch*>::const_iterator touchIter = touches.begin();
	Touch *pTouch = (Touch*)(*touchIter);
	Point location = pTouch->getLocation();
	Rect rect = _plane->getBoundingBox();
	if(rect.containsPoint(location))
	{
		isFlying = true;
	}
	log("onTouchesBegan");
}

void FlyScene::onTouchesMoved(const vector<Touch*>& touches, Event *unused_event)
{
	vector<Touch*>::const_iterator touchIter = touches.begin();
	Touch *pTouch = (Touch*)(*touchIter);
	Point location = pTouch->getLocation();

	if(isFlying)
	{
		Size size = Director::getInstance()->getWinSize();
		Rect screen = Rect(0, 0, size.width, size.height);
		if(screen.containsPoint(location)) {
			_plane->setPosition(location);
		}
	}
	log("onTouchesMoved");
}

void FlyScene::onTouchesEnded(const vector<Touch*>& touches, Event *unused_event)
{
	isFlying = false;
	log("onTouchesEnded");
}


void FlyScene::createBullet( float dt )
{
	if (_bullets->count() >= g_bulletNum)
	{
		return;
	}

	auto textureCache = Director::getInstance()->getTextureCache();
	const char *bullet_name = rand()%2 > 0 ? "bullet_1.png" : "bullet_2.png";
	auto bulletTexture = textureCache->addImage(bullet_name);

	auto bullet = Sprite::createWithTexture(bulletTexture);
	this->addChild(bullet, 0);
	_bullets->addObject(bullet);

	float x, y;
	int speedX, speedY;
	Size size = Director::getInstance()->getWinSize();

	int direction = abs(rand() % 4);
	int speedUnit = 6;
	switch(direction)
	{
	case SCREEN_DIR_UP:
		{
			x = abs(rand()%(int)size.width);
			y = size.height - bullet->getContentSize().height;
			speedX = rand()%speedUnit;
			speedY = (abs(rand()%speedUnit + 1)) * (-1);
			break;
		}

	case SCREEN_DIR_DOWN:
		{
			x = abs(rand()%(int)size.width);
			y = bullet->getContentSize().height;
			speedX = rand()%speedUnit;
			speedY = abs(rand()%speedUnit+1);
			break;
		}

	case SCREEN_DIR_LEFT:
		{
			x = bullet->getContentSize().width;
			y = abs(rand()%(int)size.height);
			speedX = abs(rand()%speedUnit+1);
			speedY=rand()%speedUnit;
			break;
		}

	case SCREEN_DIR_RIGHT:
		{
			x = size.width - bullet->getContentSize().width;
			y = abs(rand()%(int)size.height);
			speedX = abs(rand()%speedUnit+1) * (-1);
			speedY = rand()%speedUnit;
			break;
		}
	}

	bullet->setPosition(ccp(x,y));
	auto action = MoveBy::create(0.1f, ccp(speedX, speedY));
	bullet->runAction(RepeatForever::create(action));
}

void FlyScene::checkBullet(float dt)
{
	if (!_plane->isVisible())
	{
		return;
	}

	Size size = Director::getInstance()->getWinSize();
	Rect screen = Rect(0, 0, size.width, size.height);
	Rect planeBox = _plane->getBoundingBox();
	Object *bulletObj = NULL;
	Array *will_delete_bullets = Array::create();
	CCARRAY_FOREACH(_bullets, bulletObj)
	{
		Sprite *bullet = (Sprite*)bulletObj;
		Point position = bullet->getPosition();
		Rect bulletBox = bullet->getBoundingBox();

		if (planeBox.intersectsRect(bulletBox))
		{
			will_delete_bullets->addObject(bullet);
			this->removeChild(bullet, true);
			_plane->setVisible(false);

			auto textureCache = Director::getInstance()->getTextureCache();
			auto texture = textureCache->addImage("explosion.png");
					
			auto animation = Animation::create();
			animation->setDelayPerUnit(0.2f);
			for(int i=0; i<4; ++ i) {
				animation->addSpriteFrameWithTexture(texture, Rect(i*32, 0, 32, 32));
			}
				
			Animate *animate = Animate::create(animation);
			FiniteTimeAction *animateOver = CallFunc::create(this, callfunc_selector(FlyScene::explosionEndDid));
			FiniteTimeAction *seq = Sequence::create(animate, animateOver, NULL);

			_explosion = Sprite::createWithTexture(texture,Rect(0,0,32,32));
			this->addChild(_explosion);
			_explosion->setPosition(_plane->getPosition());
			_explosion->runAction(seq);			

			//if(g_isPlaySoundEffect) {
				//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("explosion.wav");
			//}
			
			break;
		}else if (!screen.containsPoint(position)) {
			this->removeChild(bullet, true);
			will_delete_bullets->addObject(bullet);
		}
	}

	_bullets->removeObjectsInArray(will_delete_bullets);
}

void FlyScene::saveTime( float dt )
{
	g_gameTime+=dt;
}

void FlyScene::explosionEndDid()
{
	_explosion->setVisible(false);
	auto scene = GameOverScene::scene();
	Director::getInstance()->replaceScene(TransitionFade::create(1.2f, scene));
}
