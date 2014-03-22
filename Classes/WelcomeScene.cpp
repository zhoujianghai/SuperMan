#include "WelcomeScene.h"
#include "GameScene.h"
#include "plane.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void showAds(bool show);
#endif


Scene* WelcomeScene::scene()
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

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("pig.plist");

	auto planeSprite = Plane::create();
	planeSprite->setScale(2.0f);
	planeSprite->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + titleSprite->getPositionY() - titleSprite->getContentSize().height - planeSprite->getContentSize().height / 2 - 100));
	this->addChild(planeSprite);
	//C++11之lambda表达式
	auto startBtnItem = MenuItemImage::create("", "", [](Object *sender) {
				Scene *scene = GameScene::scene();
				Director::getInstance()->replaceScene(CCTransitionCrossFade::create(1.2f,scene));
		});
	auto btnSprite = Sprite::create("btn_yellow.png");
	startBtnItem->setNormalSpriteFrame(btnSprite->getDisplayFrame());
	startBtnItem->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + startBtnItem->getContentSize().height / 2 + 100));
	auto startMenu = Menu::create(startBtnItem, NULL);
	startMenu->setPosition(Point::ZERO);
	this->addChild(startMenu);

	auto startBtnText = Sprite::create("start_game_text.png");
	startBtnText->setPosition(startBtnItem->getPosition());
	this->addChild(startBtnText);

	//背景雪花粒子效果
	auto particleEmitter = ParticleSystemQuad::create("snow.plist");
	particleEmitter->setPosition(Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height + 5));
	auto particleBatch = ParticleBatchNode::createWithTexture(particleEmitter->getTexture());
	particleBatch->addChild(particleEmitter);
	this->addChild(particleBatch, 5);

	//开始游戏按钮上的雪花粒子效果
	auto btnParticleEmitter = ParticleSystemQuad::create("btn_snow.plist");
	btnParticleEmitter->setPosition(Point(startBtnItem->getPositionX(), startBtnItem->getPositionY() - startBtnItem->getContentSize().height / 2 + 20));
	auto btnParticleBatch = ParticleBatchNode::createWithTexture(btnParticleEmitter->getTexture());
	btnParticleBatch->addChild(btnParticleEmitter);
	this->addChild(btnParticleBatch, 5);

	//沿着"开始游戏按钮"游动的粒子1
	auto starSprite1 = Sprite::create("star.png");
	starSprite1->setScale(0.4f);
	starSprite1->setPosition(Point(startBtnItem->getPosition().x - startBtnItem->getContentSize().width / 2, startBtnItem->getPosition().y - startBtnItem->getContentSize().height / 2));	
	this->addChild(starSprite1, 10);

	auto emitter1 = ParticleSystemQuad::create("btn_move.plist");
	emitter1->setPosition(Point(startBtnItem->getPosition().x - startBtnItem->getContentSize().width / 2 - 2, startBtnItem->getPosition().y  - startBtnItem->getContentSize().height / 2 + 3));
	auto emitterBatch1 = ParticleBatchNode::createWithTexture(emitter1->getTexture());
	emitterBatch1->addChild(emitter1);
	this->addChild(emitterBatch1, 5);

	float X = 2;
	auto path1 = buildParticleMovePath(X, startBtnItem->getContentSize().height, startBtnItem->getContentSize().width - 2 * X, true);
	starSprite1->runAction(path1);
	emitter1->runAction(path1->clone());

	//沿着"开始游戏按钮"游动的粒子2
	auto starSprite2 = Sprite::create("star.png");
	starSprite2->setScale(0.4f);
	starSprite2->setPosition(Point(startBtnItem->getPosition().x + startBtnItem->getContentSize().width / 2, startBtnItem->getPosition().y + startBtnItem->getContentSize().height / 2));	
	this->addChild(starSprite2, 10);

	auto emitter2 = ParticleSystemQuad::create("btn_move.plist");
	emitter2->setPosition(Point(startBtnItem->getPosition().x + startBtnItem->getContentSize().width / 2 - 2, startBtnItem->getPosition().y  + startBtnItem->getContentSize().height / 2 + 3));
	auto emitterBatch2 = ParticleBatchNode::createWithTexture(emitter2->getTexture());
	emitterBatch2->addChild(emitter2);
	this->addChild(emitterBatch2, 5);

	auto path2 = buildParticleMovePath(X, startBtnItem->getContentSize().height, startBtnItem->getContentSize().width - 2 * X, false);
	starSprite2->runAction(path2);
	emitter2->runAction(path2->clone());

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		showAds(true);
	#endif

    return true;
}

RepeatForever* WelcomeLayer::buildParticleMovePath(float controlX, float controlY, float w,  bool isClockwise)
{
	int flag = isClockwise ? -1 : 1;

	ccBezierConfig bezier1;
	bezier1.controlPoint_1 = Point(flag * controlX, 0);
	bezier1.controlPoint_2 = Point(flag * controlX, (-flag) * controlY);
	bezier1.endPosition = Point(0, (-flag) * controlY);

	auto bezierBy1 = BezierBy::create(0.8f, bezier1);

	float t_w = w;
	if(!isClockwise) {
		t_w = -w;
	}
	auto move1 = MoveBy::create(1.0f, Point(t_w, 0));

	ccBezierConfig bezier2;
	bezier2.controlPoint_1 = Point((-flag) * controlX, 0);
	bezier2.controlPoint_2 = Point((-flag) * controlX, flag * controlY);
	bezier2.endPosition = Point(0, flag * controlY);

	auto bezierBy2 = BezierBy::create(0.8f, bezier2);
	auto move2 = MoveBy::create(1.0f, Point(-t_w, 0));
	auto path = RepeatForever::create(Sequence::create(bezierBy1, move1, bezierBy2, move2, NULL));
	return path;
}

