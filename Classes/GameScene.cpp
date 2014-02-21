#include "GameScene.h"
#include "FlyScene.h"
#include "SettingScene.h"
#include "tools.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    auto sprite = Sprite::create("index_bg.jpg");
    sprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(sprite, 0);

	MenuItemFont::setFontName("Thonburi");
	MenuItemFont::setFontSize(25);
	auto newGame = MenuItemFont::create("New Game", CC_CALLBACK_1(GameLayer::menuNewCallback, this));
	auto settingGame = MenuItemFont::create("Setting", CC_CALLBACK_1(GameLayer::menuSettingCallback, this));
	auto closeGame = MenuItemFont::create("Exit Game", CC_CALLBACK_1(GameLayer::menuCloseCallback, this));

    auto menu = Menu::create(newGame, settingGame, closeGame, NULL);
	menu->alignItemsVertically();
	menu->alignItemsVerticallyWithPadding(50);
	this->addChild(menu, 1);

    return true;
}


void GameLayer::menuCloseCallback(Object* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void GameLayer::menuNewCallback(Object* pSender)
{
	Scene *scene = FlyScene::scene();
	Director::getInstance()->replaceScene(CCTransitionCrossFade::create(1.2f,scene));
}

void GameLayer::menuSettingCallback(Object* pSender)
{
	Scene *scene=SettingScene::scene();
	Director::getInstance()->replaceScene(CCTransitionFlipX::create(1.2f,scene));
}
