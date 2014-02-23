#include "WelcomeScene.h"
#include "FlyScene.h"
#include "SettingScene.h"
#include "tools.h"

USING_NS_CC;

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

// on "init" you need to initialize your instance
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

	auto titleSprite= Sprite::create("flappybird.png");
	titleSprite->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 150));
	this->addChild(titleSprite);

	auto planeSprite = Sprite::create("plane_small.png");
	planeSprite->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + titleSprite->getPositionY() - titleSprite->getContentSize().height - planeSprite->getContentSize().height / 2));
	this->addChild(planeSprite);

	auto startBtnItem = MenuItemImage::create("btn_yellow.png", "", CC_CALLBACK_1(WelcomeLayer::menuNewCallback, this));
	startBtnItem->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + startBtnItem->getContentSize().height / 2 + 100));
	auto startMenu = Menu::create(startBtnItem, NULL);
	startMenu->setPosition(Point::ZERO);
	this->addChild(startMenu);

	auto startBtnText = Sprite::create("start_game_text.png");
	startBtnText->setPosition(startBtnItem->getPosition());
	this->addChild(startBtnText);

	auto particleSprite = Sprite::create("snow.png");
	this->addChild(particleSprite);

	auto particleEmitter = ParticleSystemQuad::create("snow.plist");
	particleEmitter->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height + 5));
	auto particleBatch = ParticleBatchNode::createWithTexture(particleEmitter->getTexture());
	particleBatch->addChild(particleEmitter);
	this->addChild(particleBatch, 5);

	auto sunParticle = ParticleSun::createWithTotalParticles(20);
    sunParticle->setTexture( Director::getInstance()->getTextureCache()->addImage("fire.png") );
	sunParticle->setPosition(Point(planeSprite->getPositionX(), planeSprite->getPositionY() - planeSprite->getContentSize().height / 2 - 10));
	this->addChild(sunParticle, 10);

    return true;
}


void WelcomeLayer::menuCloseCallback(Object* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void WelcomeLayer::menuNewCallback(Object* pSender)
{
	Scene *scene = FlyScene::scene();
	Director::getInstance()->replaceScene(CCTransitionCrossFade::create(1.2f,scene));
}

