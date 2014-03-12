#include "WelcomeScene.h"
#include "GameScene.h"
#include "plane.h"

USING_NS_CC;


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void showAds(bool show);
#endif

Scene* WelcomeScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = WelcomeLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool WelcomeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    auto bgSprite = Sprite::create("index_bg.jpg");
    bgSprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(bgSprite, 0);

	auto titleSprite= Sprite::create("LOGO.png");
	titleSprite->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - titleSprite->getContentSize().height / 2 - 160));
	this->addChild(titleSprite);

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("resources.plist");

	auto planeSprite = Plane::create();
	planeSprite->setScale(2.0f);
	planeSprite->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + titleSprite->getPositionY() - titleSprite->getContentSize().height - planeSprite->getContentSize().height / 2 - 100));
	this->addChild(planeSprite);
	//C++11之lambda表达式
	auto startBtnItem = MenuItemImage::create("", "", [](Object *sender) {
				Scene *scene = GameScene::scene();
				Director::getInstance()->replaceScene(CCTransitionCrossFade::create(1.2f,scene));
		});
	auto btnSprite = Sprite::createWithSpriteFrameName("btn_yellow.png");
	startBtnItem->setNormalSpriteFrame(btnSprite->getDisplayFrame());
	startBtnItem->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + startBtnItem->getContentSize().height / 2 + 100));
	auto startMenu = Menu::create(startBtnItem, NULL);
	startMenu->setPosition(Point::ZERO);
	this->addChild(startMenu);

	auto startBtnText = Sprite::createWithSpriteFrameName("start_game_text.png");
	startBtnText->setPosition(startBtnItem->getPosition());
	this->addChild(startBtnText);
	//开始游戏按钮上的雪花粒子效果
	auto btnParticleEmitter = ParticleSystemQuad::create("btn_snow.plist");
	btnParticleEmitter->setPosition(Point(startBtnItem->getPositionX(), startBtnItem->getPositionY() - startBtnItem->getContentSize().height / 2 + 20));
	auto btnParticleBatch = ParticleBatchNode::createWithTexture(btnParticleEmitter->getTexture());
	btnParticleBatch->addChild(btnParticleEmitter);
	this->addChild(btnParticleBatch, 5);

	//背景雪花粒子效果
	auto particleEmitter = ParticleSystemQuad::create("snow.plist");
	particleEmitter->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height + 5));
	auto particleBatch = ParticleBatchNode::createWithTexture(particleEmitter->getTexture());
	particleBatch->addChild(particleEmitter);
	this->addChild(particleBatch, 5);

	//沿着"开始游戏按钮"游动的粒子1
	auto starSprite = Sprite::create("star.png");
	starSprite->setScale(0.4f);
	starSprite->setPosition(Point(startBtnItem->getPosition().x - startBtnItem->getContentSize().width / 2, startBtnItem->getPosition().y - startBtnItem->getContentSize().height / 2));	
	this->addChild(starSprite, 10);

	auto emitter_1 = initParticle();
	emitter_1->setPosition(Point(startBtnItem->getPosition().x - startBtnItem->getContentSize().width / 2 - 2, startBtnItem->getPosition().y  - startBtnItem->getContentSize().height / 2 + 3));

	float X = 2;
	auto path = buildParticleMovePath(X, startBtnItem->getContentSize().height, startBtnItem->getContentSize().width - 2 * X, true);

	starSprite->runAction(path);
	emitter_1->runAction(path->clone());
	//沿着"开始游戏按钮"游动的粒子2
	auto starSprite2 = Sprite::create("star.png");
	starSprite2->setScale(0.4f);
	starSprite2->setPosition(Point(startBtnItem->getPosition().x + startBtnItem->getContentSize().width / 2, startBtnItem->getPosition().y + startBtnItem->getContentSize().height / 2));	
	this->addChild(starSprite2, 10);

	auto emitter_2 = initParticle();
	emitter_2->setPosition(Point(startBtnItem->getPosition().x + startBtnItem->getContentSize().width / 2 - 2, startBtnItem->getPosition().y  + startBtnItem->getContentSize().height / 2 + 3));
	auto path2 = buildParticleMovePath(X, startBtnItem->getContentSize().height, startBtnItem->getContentSize().width - 2 * X, false);

	starSprite2->runAction(path2);
	emitter_2->runAction(path2->clone());


	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		showAds(true);
	#endif

    return true;
}

ParticleSystem* WelcomeLayer::initParticle(){
	auto emitter = new ParticleSystemQuad();
	emitter->initWithTotalParticles(100);
	addChild(emitter, 10);
	emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("star.png"));
	emitter->setAnchorPoint(Point(0, 0));
	// duration
	emitter->setDuration(ParticleSystem::DURATION_INFINITY);

	// radius mode
	emitter->setEmitterMode(ParticleSystem::Mode::RADIUS);

	// radius mode: start and end radius in pixels
	emitter->setStartRadius(4);
	emitter->setStartRadiusVar(1);
	emitter->setEndRadius(ParticleSystem::START_RADIUS_EQUAL_TO_END_RADIUS);
	emitter->setEndRadiusVar(0);

	// radius mode: degrees per second
	emitter->setRotatePerSecond(100);
	emitter->setRotatePerSecondVar(0);

	// angle
	emitter->setAngle(90);
	emitter->setAngleVar(0);

	// emitter position
	auto size = Director::getInstance()->getWinSize();
	emitter->setPosVar(Point::ZERO);

	// life of particles
	emitter->setLife(1.0);
	emitter->setLifeVar(0);

	// spin of particles
	emitter->setStartSpin(0);
	emitter->setStartSpinVar(0);
	emitter->setEndSpin(0);
	emitter->setEndSpinVar(0);

	// color of particles
	Color4F startColor(1.0f, 1.0f, 0.9f, 1.0f);
	emitter->setStartColor(startColor);

	Color4F startColorVar(0, 0, 0, 1.0f);
	emitter->setStartColorVar(startColorVar);

	Color4F endColor(1.0f, 1.0f, 1.0f, 0.1f);
	emitter->setEndColor(endColor);

	Color4F endColorVar(0, 0, 0, 0.1f);
	emitter->setEndColorVar(endColorVar);

	// size, in pixels
	emitter->setStartSize(15);
	emitter->setStartSizeVar(1);
	emitter->setEndSize(0);

	// emits per second
	emitter->setEmissionRate(emitter->getTotalParticles() / emitter->getLife());

	// additive
	emitter->setBlendAdditive(false);

	return emitter;
}

RepeatForever* WelcomeLayer::buildParticleMovePath(float controlX, float controlY, float w,  bool isClockwise)
{
	ccBezierConfig bezier1;
	if(isClockwise) {
		bezier1.controlPoint_1 = Point(-controlX, 0);
		bezier1.controlPoint_2 = Point(-controlX, controlY);
		bezier1.endPosition = Point(0, controlY);
	} else {
			bezier1.controlPoint_1 = Point(controlX, 0);
			bezier1.controlPoint_2 = Point(controlX, -controlY);
			bezier1.endPosition = Point(0, -controlY);
	}

	auto bezierBy1 = BezierBy::create(0.8f, bezier1);

	float t_w = w;
	if(!isClockwise) {
		t_w = -w;
	}
	auto move1 = MoveBy::create(1.0f, Point(t_w, 0));

	ccBezierConfig bezier2;
	if(isClockwise) {
			bezier2.controlPoint_1 = Point(controlX, 0);
	bezier2.controlPoint_2 = Point(controlX, -controlY);
	bezier2.endPosition = Point(0, -controlY);
	}else {
					bezier2.controlPoint_1 = Point(-controlX, 0);
	bezier2.controlPoint_2 = Point(-controlX, controlY);
	bezier2.endPosition = Point(0, controlY);
	}

	auto bezierBy2 = BezierBy::create(0.8f, bezier2);
	auto move2 = MoveBy::create(1.0f, Point(-t_w, 0));
	auto path = RepeatForever::create(Sequence::create(bezierBy1, move1, bezierBy2, move2, NULL));
	return path;
}

