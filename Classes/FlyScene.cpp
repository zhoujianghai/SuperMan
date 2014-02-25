#include "FlyScene.h"
#include "WelcomeScene.h"
#include "GameOverScene.h"
#include "global.h"
#include "tools.h"
#include "bullet.h"
#include "plane.h"
#include "VisibleRect.h"
#include "MyContactListener.h"


using namespace cocos2d;
using namespace std;

#define PTM_RATIO 32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void showAds(bool show);
#endif

FlyScene::FlyScene()
	:_world(NULL),
	_contactListener(NULL),
	_isGameOver(false),
	m_pJoystick(NULL),
	m_pJoystickBg(NULL)
{

}

FlyScene::~FlyScene()
{
	CC_SAFE_DELETE(_contactListener);
	CC_SAFE_DELETE(_world);
}

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


		_scoreLabel =  LabelAtlas::create("0", "img_num_dis.png", 22, 28, '0');
		_scoreLabel->setPosition(Point(origin.x + visibleSize.width / 2 - _scoreLabel->getContentSize().width / 2, origin.y + visibleSize.height - _scoreLabel->getContentSize().height - 5));
		this->addChild(_scoreLabel);

		this->initPhysics();

		_plane = Plane::create();
		_plane->setTag(SPRITE_PLANE);
		_plane->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
		//_sprite_batch->addChild(_plane, 100);
		this->addChild(_plane, 100);
		this->addBoxBodyForSprite(_plane);

		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(FlyScene::onTouchesBegan, this);
		listener->onTouchesMoved = CC_CALLBACK_2(FlyScene::onTouchesMoved, this);
		listener->onTouchesEnded = CC_CALLBACK_2(FlyScene::onTouchesEnded, this); 
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);



		this->schedule(schedule_selector(FlyScene::createBullet));
		this->schedule(schedule_selector(FlyScene::updateBullet));
		this->schedule(schedule_selector(FlyScene::updatePlane));
		this->schedule(schedule_selector(FlyScene::updateBoxBody));
		this->schedule(SEL_SCHEDULE(&FlyScene::updateScore),1);

		isFlying = false;
		g_gameTime = 0;
		_bullets = Array::create();
		_bullets->retain();

		m_pJoystick = Sprite::create("joystick.png");
		m_pJoystickBg = Sprite::create("joystick_bg.png");
		this->addChild(m_pJoystickBg, 0);
		this->addChild(m_pJoystick, 1);

		this->hideJoystick();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		showAds(false);
	#endif

		bRet = true;
	}while(0);

	return bRet;
}

void FlyScene::initPhysics()
{
	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	_world = new b2World(gravity);
	_world->SetAllowSleeping(false);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);
	b2Body *groundBody = _world->CreateBody(&groundBodyDef);
	b2EdgeShape groundBox;
	//bottom
	groundBox.Set(b2Vec2(VisibleRect::leftBottom().x / PTM_RATIO, VisibleRect::leftBottom().y / PTM_RATIO), b2Vec2(VisibleRect::rightBottom().x / PTM_RATIO, VisibleRect::rightBottom().y / PTM_RATIO));
	groundBody->CreateFixture(&groundBox, 0);
	//right
	groundBox.Set(b2Vec2(VisibleRect::rightBottom().x / PTM_RATIO, VisibleRect::rightBottom().y / PTM_RATIO), b2Vec2(VisibleRect::rightTop().x / PTM_RATIO, VisibleRect::rightTop().y / PTM_RATIO));
	groundBody->CreateFixture(&groundBox, 0);
	//top
	groundBox.Set(b2Vec2(VisibleRect::leftTop().x / PTM_RATIO, VisibleRect::leftTop().y / PTM_RATIO), b2Vec2(VisibleRect::rightTop().x / PTM_RATIO, VisibleRect::rightTop().y / PTM_RATIO));
	groundBody->CreateFixture(&groundBox, 0);
	//left
	groundBox.Set(b2Vec2(VisibleRect::leftBottom().x / PTM_RATIO, VisibleRect::leftBottom().y / PTM_RATIO), b2Vec2(VisibleRect::leftTop().x / PTM_RATIO, VisibleRect::leftTop().y / PTM_RATIO));
	groundBody->CreateFixture(&groundBox, 0);

	_contactListener = new MyContactListener();
	_world->SetContactListener(_contactListener);
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
	}else {
		this->showJoystick(location);
	}

	
	//log("onTouchesBegan");
}

void FlyScene::onTouchesMoved(const vector<Touch*>& touches, Event *unused_event)
{
	vector<Touch*>::const_iterator touchIter = touches.begin();
	Touch *pTouch = (Touch*)(*touchIter);
	Point location = pTouch->getLocation();

		Point start = pTouch->getStartLocation();
	float distance = start.getDistance(location);
	Point direction = (location - start).normalize();
	this->updateJoystick(direction, distance);
		Size size = Director::getInstance()->getWinSize();
		Rect screen = Rect(0, 0, size.width, size.height);
	if(isFlying)
	{
		if(screen.containsPoint(location)) {
			_plane->setPosition(location);
		}
	}else {
			Point velocity = direction * (distance < 78 ? 3 : 5);
			_plane->setVelocity(velocity);


	}


	//log("onTouchesMoved");
}

void FlyScene::onTouchesEnded(const vector<Touch*>& touches, Event *unused_event)
{
	isFlying = false;
	this->hideJoystick();
	//log("onTouchesEnded");
}


void FlyScene::createBullet( float dt )
{
	if (_bullets->count() >= g_bulletNum)
	{
		return;
	}

	Bullet* bullet = Bullet::create();
	float scale = ((rand() % 10) + 6) / 10.0f;
	if(scale < 0.6f) scale = 0.6f;
	bullet->setScale(scale);
	bullet->setTag(SPRITE_BULLET);
	_sprite_batch->addChild(bullet);
	_bullets->addObject(bullet);

	//log("createBullet bullet=%p", bullet);
	this->addBoxBodyForSprite(bullet);
}

void FlyScene::showJoystick(Point pos)
{
	m_pJoystick->setPosition(pos);
	m_pJoystickBg->setPosition(pos);

	m_pJoystick->setVisible(true);
	m_pJoystickBg->setVisible(true);
}

void FlyScene::hideJoystick()
{
	m_pJoystick->setPosition(m_pJoystickBg->getPosition());
	m_pJoystick->setVisible(false);
	m_pJoystickBg->setVisible(false);
}

void FlyScene::updateJoystick(Point direction, float distance)
{
	Point start = m_pJoystickBg->getPosition();

	if(distance < 33)
	{
		m_pJoystick->setPosition(start + (direction * distance));
	}else if(distance > 78) {
		m_pJoystick->setPosition(start + (direction * 45));
	}else {
		m_pJoystick->setPosition(start + (direction * 33));
	}
}

void FlyScene::updatePlane(float dt)
{
	if(m_pJoystick && m_pJoystick->isVisible()) {
				Size size = Director::getInstance()->getWinSize();
		Rect screen = Rect(0, 0, size.width, size.height);
		Point planePos = _plane->getPosition();
		Point expectP = planePos + _plane->getVelocity();
			if(screen.containsPoint(expectP)) {
			_plane->setPosition(expectP);
		}
	}

}

void FlyScene::updateBoxBody(float dt)
{
	if (_isGameOver)
	{
		return;
	}

	//调用world对象的step方法，这样它就可以进行物理仿真了。这里的两个参数分别是“速度迭代次数”和“位置迭代次数”--你应该设置他们的范围在8-10之间。
	//这里的数字越小，精度越小，但是效率更高。数字越大，仿真越精确，但同时耗时更多。8一般是个折中
	_world->Step(dt, 10, 10);

	std::vector<b2Body *> toDestroy;
	Size size = Director::getInstance()->getWinSize();
	Rect screen = Rect(0, 0, size.width, size.height);

	for(b2Body *body = _world->GetBodyList(); body; body = body->GetNext()) {
		if(body->GetUserData() != NULL) {
			Sprite *sprite = (Sprite*)body->GetUserData();
			b2Vec2 b2Pos = b2Vec2(sprite->getPositionX() / PTM_RATIO, sprite->getPositionY() / PTM_RATIO);
			float b2Angle = -1 * CC_DEGREES_TO_RADIANS(sprite->getRotation());
			body->SetTransform(b2Pos, b2Angle);

			if (sprite->getTag() == SPRITE_BULLET && !screen.containsPoint(sprite->getPosition())) {
				toDestroy.push_back(body);
				_bullets->removeObject(sprite);
			}
		 }
	}

	std::vector<MyContact>::iterator iter;
	for(iter = _contactListener->_contacts.begin(); iter != _contactListener->_contacts.end(); ++ iter) {
		MyContact contact = *iter;
		b2Body *bodyA = contact.fixtureA->GetBody();
		b2Body *bodyB = contact.fixtureB->GetBody();
		if(bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL) {
			Sprite *spriteA = (Sprite*)bodyA->GetUserData();
			Sprite *spriteB = (Sprite*)bodyB->GetUserData();

			if(spriteA->getTag() == SPRITE_PLANE && spriteB->getTag() == SPRITE_BULLET) {
				toDestroy.push_back(bodyA);
				_isGameOver = true;
			}else if(spriteB->getTag() == SPRITE_PLANE && spriteA->getTag() == SPRITE_BULLET) {
				toDestroy.push_back(bodyB);
				_isGameOver = true;
			}
		}
	}
	std::vector<b2Body *>::iterator iter2;
	for(iter2 = toDestroy.begin(); iter2 != toDestroy.end(); ++ iter2) {
		b2Body *body = *iter2;
		if(body->GetUserData() != NULL) {
			Sprite *sprite = (Sprite *)body->GetUserData();
			//log("sprite=%p", sprite);
			// just do it temp
			if(sprite->getTag() == SPRITE_BULLET) {
				_sprite_batch->removeChild(sprite, true);
			}
		}
		_world->DestroyBody(body);
	}

	if(toDestroy.size() > 0 && _isGameOver) {
		g_gameTime = 0;
		_plane->setVisible(false);
		_scoreLabel->setVisible(false);

		auto particleSprite = Sprite::create("explosion.png");
		this->addChild(particleSprite);

		auto particleEmitter = ParticleSystemQuad::create("explosion.plist");
		particleEmitter->setPosition(_plane->getPosition());
		auto particleBatch = ParticleBatchNode::createWithTexture(particleEmitter->getTexture());
		particleBatch->addChild(particleEmitter);
		this->addChild(particleBatch, 10);

		_sprite_batch->removeAllChildrenWithCleanup(true);
		_sprite_batch->setVisible(false);
		_bullets->removeAllObjects();
		this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(FlyScene::explosionEndDid, this)),  NULL));
	}
}

void FlyScene::updateBullet(float dt)
{
	if (_isGameOver)
	{
		return;
	}

	Object *bulletObj = NULL;
	CCARRAY_FOREACH(_bullets, bulletObj)
	{
		Bullet *bullet = (Bullet*)bulletObj;
		Point position = bullet->getPosition();

		Point new_pos = Point(position.x + bullet->get_speed_x(), position.y + bullet->get_speed_y());
		bullet->setPosition(new_pos);
	}
}

void FlyScene::updateScore( float dt )
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

void FlyScene::addBoxBodyForSprite(cocos2d::Sprite *sprite)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(sprite->getPositionX() / PTM_RATIO, sprite->getPositionY() / PTM_RATIO);
	bodyDef.userData = sprite;
	b2Body *body = _world->CreateBody(&bodyDef);

	b2PolygonShape spriteShape;
	if(sprite->getTag() == SPRITE_PLANE) {
		int num = 8;
		b2Vec2 verts[] = {
			b2Vec2(-6.0f / PTM_RATIO, 45.0f / PTM_RATIO),
			b2Vec2(-42.0f / PTM_RATIO, 4.0f / PTM_RATIO),
			b2Vec2(-56.0f / PTM_RATIO, -23.0f / PTM_RATIO),
			b2Vec2(-17.0f / PTM_RATIO, -45.0f / PTM_RATIO),
			b2Vec2(19.0f / PTM_RATIO, -44.0f / PTM_RATIO),
			b2Vec2(56.0f / PTM_RATIO, -18.0f / PTM_RATIO),
			b2Vec2(43.0f / PTM_RATIO, 5.0f / PTM_RATIO),
			b2Vec2(5.0f / PTM_RATIO,46.0 / PTM_RATIO)
		};
		spriteShape.Set(verts, num);
	}else if(sprite->getTag() == SPRITE_BULLET) {
		int num = 8;
		b2Vec2 verts[] = {
			b2Vec2(-0.3f / PTM_RATIO, 7.0f / PTM_RATIO),
			b2Vec2(-6.3f / PTM_RATIO, 5.4f / PTM_RATIO),
			b2Vec2(-7.9f / PTM_RATIO, -0.2f / PTM_RATIO),
			b2Vec2(-5.6f / PTM_RATIO, -4.8f / PTM_RATIO),
			b2Vec2(-0.8f / PTM_RATIO, -6.5f / PTM_RATIO),
			b2Vec2(5.8f / PTM_RATIO, -4.8f / PTM_RATIO),
			b2Vec2(7.3f / PTM_RATIO, 0.8f / PTM_RATIO),
			b2Vec2(5.2f / PTM_RATIO, 5.6 / PTM_RATIO)
		};
		spriteShape.Set(verts, num);
	}

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &spriteShape;
	fixtureDef.density = 10.0f;
	fixtureDef.isSensor = true;

	body->CreateFixture(&fixtureDef);
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


		//C++11之lambda表达式
		auto startBtnItem = MenuItemImage::create("btn_yellow.png", "btn_yellow_pressed.png", [](Object *sender) {
				Scene *scene = FlyScene::scene();
				Director::getInstance()->replaceScene(scene);
		});
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

}
