#include "GameScene.h"
#include "WelcomeScene.h"
#include "bullet.h"
#include "plane.h"
#include "VisibleRect.h"
#include "MyContactListener.h"
#include "XmlParser.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "extensions/cocos-ext.h"
#else
#include "cocos-ext.h"
#endif

USING_NS_CC_EXT;


using namespace cocos2d;
using namespace std;

#define PTM_RATIO 32

int g_gameTime = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void showAds(bool show);
#endif

string getLevel(int score) {
	string ret = "";
	if(score < 20) {
		ret = "xiaobing";
	}else if(score < 30) {
		ret = "banzhang";
	}else if(score < 40) {
		ret = "paizhang";
	}else if(score < 50) {
		ret = "lianzhang";
	}
	else if(score < 60) {
		ret = "yingzhang";
	}else if(score < 70) {
		ret = "tuanzhang";
	}else if(score < 80) {
		ret = "lvzhang";
	}else if(score < 90) {
		ret = "shizhang";
	}else if(score < 100) {
		ret = "junzhang";
	}else if(score >= 120) {
		ret = "siling";
	}

	return ret;
}

int getBulletNum(int score) {
	int ret = 10 + score;
	if(ret > 100) {
		ret = 100;
	}

	return ret;
}

GameScene::GameScene()
	:_world(NULL),
	_contactListener(NULL),
	_isGameOver(false),
	m_pJoystick(NULL),
	m_pJoystickBg(NULL)
{

}

GameScene::~GameScene()
{
	CC_SAFE_DELETE(_contactListener);
	CC_SAFE_DELETE(_world);
}

bool GameScene::init()
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

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("resources.plist");
		_sprite_batch = SpriteBatchNode::create("resources.png");
		this->addChild(_sprite_batch);

		_scoreLabel =  LabelAtlas::create("0", "number_small.png", 22, 28, '0');
		_scoreLabel->setPosition(Point(origin.x + visibleSize.width / 2 - _scoreLabel->getContentSize().width / 2, origin.y + visibleSize.height - _scoreLabel->getContentSize().height - 5));
		this->addChild(_scoreLabel);

		this->initPhysics();

		_plane = Plane::create();
		_plane->setTag(SPRITE_PLANE);
		_plane->setScale(2.0f);
		_plane->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
		//_sprite_batch->addChild(_plane, 100);
		this->addChild(_plane, 100);
		this->addBoxBodyForSprite(_plane);

		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
		listener->onTouchesMoved = CC_CALLBACK_2(GameScene::onTouchesMoved, this);
		listener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this); 
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);



		this->schedule(schedule_selector(GameScene::createBullet));
		this->schedule(schedule_selector(GameScene::updateBullet));
		this->schedule(schedule_selector(GameScene::updatePlane));
		this->schedule(schedule_selector(GameScene::updateBoxBody));
		this->schedule(SEL_SCHEDULE(&GameScene::updateScore),1);

		isFlying = false;
		g_gameTime = 0;
		_bullets = Array::create();
		_bullets->retain();

		m_pJoystick = Sprite::createWithSpriteFrameName("joystick.png");
		m_pJoystickBg = Sprite::createWithSpriteFrameName("joystick_bg.png");
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

void GameScene::initPhysics()
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

void GameScene::pausePressed(cocos2d::Object *pSender)
{
	auto scene = WelcomeScene::createScene();
	Director::sharedDirector()->replaceScene(TransitionFlipX::create(1.2f, scene));
}

Scene* GameScene::scene()
{
	Scene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = Scene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		GameScene *layer = GameScene::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

void GameScene::menuReturnCallback( Object *pSender )
{

}

void GameScene::onTouchesBegan(const vector<Touch*>& touches, Event *unused_event)
{
	if(_isGameOver) return;

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

void GameScene::onTouchesMoved(const vector<Touch*>& touches, Event *unused_event)
{
	if(_isGameOver) return;

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

void GameScene::onTouchesEnded(const vector<Touch*>& touches, Event *unused_event)
{
	if(_isGameOver) return;

	isFlying = false;
	this->hideJoystick();
	//log("onTouchesEnded");
}


void GameScene::createBullet( float dt )
{
	int maxBulletNum = getBulletNum(g_gameTime);
	if (_bullets->count() >= maxBulletNum)
	{
		return;
	}

	Bullet* bullet = Bullet::create();
	bullet->setScale(0.5f);
	bullet->setTag(SPRITE_BULLET);
	_sprite_batch->addChild(bullet);
	_bullets->addObject(bullet);

	//log("createBullet bullet=%p", bullet);
	this->addBoxBodyForSprite(bullet);
}

void GameScene::showJoystick(Point pos)
{
	m_pJoystick->setPosition(pos);
	m_pJoystickBg->setPosition(pos);

	m_pJoystick->setVisible(true);
	m_pJoystickBg->setVisible(true);
}

void GameScene::hideJoystick()
{
	m_pJoystick->setPosition(m_pJoystickBg->getPosition());
	m_pJoystick->setVisible(false);
	m_pJoystickBg->setVisible(false);
}

void GameScene::updateJoystick(Point direction, float distance)
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

void GameScene::updatePlane(float dt)
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

void GameScene::updateBoxBody(float dt)
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
		this->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(GameScene::explosionEndDid, this)),  NULL));
	}
}

void GameScene::updateBullet(float dt)
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

void GameScene::updateScore( float dt )
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

void GameScene::explosionEndDid()
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

void GameScene::addBoxBodyForSprite(cocos2d::Sprite *sprite)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(sprite->getPositionX() / PTM_RATIO, sprite->getPositionY() / PTM_RATIO);
	bodyDef.userData = sprite;
	b2Body *body = _world->CreateBody(&bodyDef);

	if(sprite->getTag() == SPRITE_PLANE) {
		int num = 5;
		b2Vec2 verts[] = {
			b2Vec2(-10.9f / PTM_RATIO, 24.3f / PTM_RATIO),
			b2Vec2(-25.6f / PTM_RATIO, 0.0f / PTM_RATIO),
			b2Vec2(-1.6f / PTM_RATIO, -24.0f / PTM_RATIO),
			b2Vec2(26.4f / PTM_RATIO, 2.4f / PTM_RATIO),
			b2Vec2(10.4f / PTM_RATIO, 24.8f / PTM_RATIO)
		};
		b2FixtureDef fixtureDef;
		b2PolygonShape spriteShape;
		spriteShape.Set(verts, num);
		fixtureDef.shape = &spriteShape;
		fixtureDef.density = 10.0f;
		fixtureDef.isSensor = true;
		body->CreateFixture(&fixtureDef);

	}else if(sprite->getTag() == SPRITE_BULLET) {
		b2FixtureDef fixtureDef;
		b2CircleShape spriteShape;
		spriteShape.m_radius = 40.0f / PTM_RATIO;
		fixtureDef.shape = &spriteShape;
		fixtureDef.density = 10.0f;
		fixtureDef.isSensor = true;
		body->CreateFixture(&fixtureDef);
	}


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
		
		auto title = Sprite::create("girl.png");
		title->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - title->getContentSize().height / 2 - 100));
		this->addChild(title);

		auto scorePanel = Scale9Sprite::create("content_bg.png");
		scorePanel->setPreferredSize(Size(500, 250));
		scorePanel->setPosition(Point(origin.x + visibleSize.width / 2, title->getPositionY() - title->getContentSize().height / 2 - scorePanel->getContentSize().height / 2 - 80));

		auto purpleBase = Sprite::create("purple_base.png");
		purpleBase->setScaleY(1.2f);
		purpleBase->setScaleX(1.3f);
		purpleBase->setPosition(Point(origin.x + purpleBase->getContentSize().width / 2, scorePanel->getPositionY() + scorePanel->getContentSize().height / 2 + 60));
	
		char tmp2[4];
		sprintf(tmp2, "%d", g_gameTime);
		string scoreStr(tmp2);
		auto newScoreLabel =  LabelAtlas::create(scoreStr.c_str(), "number_large.png", 64, 90, '0');
		newScoreLabel->setScale(0.8f);
		newScoreLabel->setPosition(Point(purpleBase->getContentSize().width / 2 - newScoreLabel->getContentSize().width / 2 + 20, purpleBase->getContentSize().height / 2 - newScoreLabel->getContentSize().height / 2 + 30));
		purpleBase->addChild(newScoreLabel);
		//newScoreLabel->setScale(0.9f);

		auto newRecordText = Sprite::createWithSpriteFrameName("record_breaking.png");
		newRecordText->setPosition(Point(purpleBase->getContentSize().width / 2, purpleBase->getContentSize().height - newRecordText->getContentSize().height / 2));
		purpleBase->addChild(newRecordText);

		auto levelText = Sprite::create("level.png");
		levelText->setPosition(Point(levelText->getContentSize().width / 2 + 140, scorePanel->getContentSize().height - levelText->getContentSize().height - 30));
		levelText->setScale(1.5f);
		scorePanel->addChild(levelText);

		auto userDefault = UserDefault::getInstance();
		int oldScore = userDefault->getIntegerForKey("score");
		if(g_gameTime <= oldScore) {
			newRecordText->setVisible(false);
		}
		int maxScore = g_gameTime > oldScore ? g_gameTime : oldScore;
		userDefault->setIntegerForKey("score", maxScore);

		XMLParser *pXmlParser = XMLParser::parseWithFile("level.xml");
		string level = ::getLevel(maxScore);
		CCString *pValue1 = pXmlParser->getString(level.c_str());
		auto levelDescText = LabelTTF::create(pValue1->getCString(), "Arial", 40);
		levelDescText->setColor(Color3B(220, 145, 39));
		levelDescText->setPosition(Point(levelText->getPositionX() + levelText->getContentSize().width + 60, levelText->getPositionY()));
		scorePanel->addChild(levelDescText);

		auto maxText = Sprite::create("max.png");
		maxText->setScale(1.5f);
		maxText->setPosition(Point(levelText->getPositionX(), levelText->getPositionY() - levelText->getContentSize().height - 50));
		scorePanel->addChild(maxText);



		char tmp[4];
		sprintf(tmp, "%d", maxScore);
		string maxScoreStr(tmp);

		auto maxScoreLabel =  LabelAtlas::create(maxScoreStr.c_str(), "number_large.png", 64, 90, '0');
		maxScoreLabel->setPosition(Point(levelDescText->getPositionX() - 30, maxText->getPositionY() - maxText->getContentSize().height / 2 - 10));
		maxScoreLabel->setScale(0.6f);
		scorePanel->addChild(maxScoreLabel);

		this->addChild(scorePanel);
		this->addChild(purpleBase);


		//C++11之lambda表达式
		auto startBtnItem = MenuItemImage::create("", "", [](Object *sender) {
				Scene *scene = GameScene::scene();
				Director::getInstance()->replaceScene(scene);
		});

		auto btnSprite = Sprite::createWithSpriteFrameName("btn_yellow.png");
		startBtnItem->setNormalSpriteFrame(btnSprite->getDisplayFrame());
		startBtnItem->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + startBtnItem->getContentSize().height / 2 + 150));
		auto startMenu = Menu::create(startBtnItem, NULL);
		startMenu->setPosition(Point::ZERO);
		this->addChild(startMenu);

		auto startBtnText = Sprite::createWithSpriteFrameName("again_game_text.png");
		startBtnText->setPosition(startBtnItem->getPosition());
		this->addChild(startBtnText);

		ret = true;
	}while(0);

	return ret;
}

void GameOverLayer::menuNewCallback(Object* pSender)
{

}
