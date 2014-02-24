#include "FlyScene.h"
#include "WelcomeScene.h"
#include "GameOverScene.h"
#include "global.h"
#include "tools.h"
#include "bullet.h"
#include "plane.h"

using namespace cocos2d;
using namespace std;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void showAds(bool show);
#endif

bool FlyScene::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(! Layer::init());

		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto origin = Director::getInstance()->getVisibleOrigin();
		int index = (int)(rand() % 5 + 1);
		const char* bg_name = String::createWithFormat("img_bg_%d.jpg", index)->getCString();
		//log("bg_name=%s", bg_name);
		auto background = Sprite::create(bg_name);
		background->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
		this->addChild(background, -1);


		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("commons.plist");
		_sprite_batch = SpriteBatchNode::create("commons.png");
		this->addChild(_sprite_batch);

		//auto pause_menu_item = MenuItemImage::create("", "", CC_CALLBACK_1(FlyScene::pausePressed, this));
		//pause_menu_item->setNormalSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("pause.png"));
		//pause_menu_item->setPosition(Point(origin.x + visibleSize.width - pause_menu_item->getContentSize().width / 2, origin.y + visibleSize.height - pause_menu_item->getContentSize().height / 2));
		//auto pauseMenu = Menu::create(pause_menu_item, NULL);
		//pauseMenu->setPosition(Point::ZERO);
		//this->addChild(pauseMenu, 1);

		_scoreLabel =  LabelAtlas::create("0", "img_num_dis.png", 22, 28, '0');
		_scoreLabel->setPosition(Point(origin.x + visibleSize.width / 2 - _scoreLabel->getContentSize().width / 2, origin.y + visibleSize.height - _scoreLabel->getContentSize().height - 5));
		this->addChild(_scoreLabel);

		_plane = Plane::create();
		_plane->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
		//_sprite_batch->addChild(_plane, 100);
		this->addChild(_plane, 100);

		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(FlyScene::onTouchesBegan, this);
		listener->onTouchesMoved = CC_CALLBACK_2(FlyScene::onTouchesMoved, this);
		listener->onTouchesEnded = CC_CALLBACK_2(FlyScene::onTouchesEnded, this); 
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		this->schedule(schedule_selector(FlyScene::createBullet));
		this->schedule(schedule_selector(FlyScene::checkBullet));
		this->schedule(SEL_SCHEDULE(&FlyScene::saveTime),1);

		isFlying = false;
		g_gameTime = 0;
		_bullets = Array::create();
		_bullets->retain();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		showAds(false);
	#endif

		bRet = true;
	}while(0);

	return bRet;
}

void FlyScene::pausePressed(cocos2d::Object *pSender)
{
	auto scene = WelcomeScene::createScene();
	Director::sharedDirector()->replaceScene(TransitionFlipX::create(1.2f, scene));
}

Scene* FlyScene::scene()
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

	Bullet* bullet = Bullet::create();
	_sprite_batch->addChild(bullet);
	_bullets->addObject(bullet);
}

void FlyScene::checkBullet(float dt)
{
	if (!_plane->isVisible() || _bullets->count() == 0)
	{
		return;
	}

	Size size = Director::getInstance()->getWinSize();
	Rect screen = Rect(0, 0, size.width, size.height);
	Rect planeBox = _plane->getBoundingBox();
	Object *bulletObj = NULL;
	Array *will_delete_bullets = Array::create();
	bool isGameOver = false;

	CCARRAY_FOREACH(_bullets, bulletObj)
	{
		Bullet *bullet = (Bullet*)bulletObj;
		Point position = bullet->getPosition();
		Rect bulletBox = bullet->getBoundingBox();

		if (planeBox.intersectsRect(bulletBox))
		{
/*			will_delete_bullets->addObject(bullet);
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
			_explosion->runAction(seq);		*/	

			//if(g_isPlaySoundEffect) {
				//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("explosion.wav");
			//}
			g_gameTime = 0;
			_scoreLabel->setVisible(false);
			_plane->setVisible(false);

			auto particleSprite = Sprite::create("explosion.png");
			this->addChild(particleSprite);

			auto particleEmitter = ParticleSystemQuad::create("explosion.plist");
			particleEmitter->setPosition(_plane->getPosition());
			auto particleBatch = ParticleBatchNode::createWithTexture(particleEmitter->getTexture());
			particleBatch->addChild(particleEmitter);
			this->addChild(particleBatch, 10);

			isGameOver = true;

			break;
		}else if (!screen.containsPoint(position)) {
			_sprite_batch->removeChild(bullet, true);
			will_delete_bullets->addObject(bullet);
			break;
		}
		Point new_pos = Point(position.x + bullet->get_speed_x(), position.y + bullet->get_speed_y());
		bullet->setPosition(new_pos);
	}

	if(will_delete_bullets->count() > 0) {
		_bullets->removeObjectsInArray(will_delete_bullets);
	}

	if(isGameOver) {
		_sprite_batch->removeAllChildren();
		_bullets->removeAllObjects();
		this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(FlyScene::explosionEndDid, this)),  NULL));
	}

}

void FlyScene::saveTime( float dt )
{
	if( _plane->isVisible()) {
		g_gameTime += dt;
		if(_scoreLabel) {
		char tmp[4];
		sprintf(tmp, "%d", g_gameTime);
		string scoreStr(tmp);
		//log("score=%s", scoreStr.c_str());
		_scoreLabel->setString(scoreStr);
		}
	}
}

void FlyScene::explosionEndDid()
{
	//_explosion->setVisible(false);
	auto scene = Director::getInstance()->getRunningScene();
	auto layer = GameOverLayer::create();
	scene->addChild(layer);

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		showAds(true);
	#endif
	//Director::getInstance()->replaceScene(TransitionFade::create(1.2f, scene));
}

GameOverLayer::GameOverLayer()
{

}

GameOverLayer::~GameOverLayer()
{

}

bool GameOverLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !this->initWithColor(Color4B(105, 105, 105, 128)) );
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Point origin = Director::getInstance()->getVisibleOrigin();
		
		auto title = Sprite::create("gameover.png");
		title->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - title->getContentSize().height / 2 - 200));
		this->addChild(title);

		auto scorePanel = Sprite::create("base.png");
		scorePanel->setPosition(Point(origin.x + visibleSize.width / 2, title->getPositionY() - title->getContentSize().height / 2 - scorePanel->getContentSize().height / 2 - 50));

		auto oldScoreLabel =  LabelAtlas::create("0", "number2.png", 54, 79, '0');
		oldScoreLabel->setPosition(Point(scorePanel->getContentSize().width - 140, 30));
		scorePanel->addChild(oldScoreLabel);
		//oldScoreLabel->setScale(0.9f);
		
		auto newScoreLabel =  LabelAtlas::create("2", "number2.png", 54, 79, '0');
		newScoreLabel->setPosition(Point(scorePanel->getContentSize().width - 140, 142));
		scorePanel->addChild(newScoreLabel);
		//newScoreLabel->setScale(0.9f);

		this->addChild(scorePanel);


		auto startBtnItem = MenuItemImage::create("btn_yellow.png", "btn_yellow_pressed.png", CC_CALLBACK_1(GameOverLayer::menuNewCallback, this));
		startBtnItem->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + startBtnItem->getContentSize().height / 2 + 150));
		auto startMenu = Menu::create(startBtnItem, NULL);
		startMenu->setPosition(Point::ZERO);
		this->addChild(startMenu);

		auto startBtnText = Sprite::create("start_game_text.png");
		startBtnText->setPosition(startBtnItem->getPosition());
		this->addChild(startBtnText);

		ret = true;
	}while(0);

	return ret;
}

void GameOverLayer::menuNewCallback(Object* pSender)
{
	Scene *scene = FlyScene::scene();
	Director::getInstance()->replaceScene(scene);
}
