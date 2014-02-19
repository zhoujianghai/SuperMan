#include "SettingScene.h"
#include "GameScene.h"
#include "tools.h"
#include "global.h"

using namespace cocos2d;

bool SettingScene::init()
{
	bool bRet = false;
	do 
	{
		MenuItemFont::setFontName("American Typewriter");
		MenuItemFont::setFontSize(18);

		//声音开关
		std::string str = "sound";

		auto title1 = MenuItemFont::create(str.c_str());
		title1->setEnabled(false);

		str = "open";

		auto openItem = MenuItemFont::create(str.c_str());

		str = "close";

		auto closeItem = MenuItemFont::create(str.c_str());

		auto soundItem = MenuItemToggle::createWithCallback( CC_CALLBACK_1(SettingScene::bulletNumCallback, this),
                                                                openItem,
                                                                closeItem,
                                                                NULL );

		soundItem->setSelectedIndex(g_isPlaySoundEffect?0:1);

		//弹数设置
		str = "dan shu";

		CCMenuItemFont *title2 = CCMenuItemFont::create(str.c_str(),NULL,NULL);
		title2->setEnabled(false);

		str = "50";
		CCMenuItemFont *bulletNum50Item = CCMenuItemFont::create(str.c_str());

		str = "100";
		CCMenuItemFont *bulletNum100Item = CCMenuItemFont::create(str.c_str());

		str = "150";
		CCMenuItemFont *bulletNum150Item = CCMenuItemFont::create(str.c_str());

	
		auto bulletNumItem = MenuItemToggle::createWithCallback( CC_CALLBACK_1(SettingScene::bulletNumCallback, this),
                                                                bulletNum50Item,
                                                                bulletNum100Item,
																bulletNum150Item,
                                                                NULL );
		bulletNumItem->setSelectedIndex(g_bulletNum/50-1);

		CCMenuItemFont::setFontName("Marker Felt");
		CCMenuItemFont::setFontSize(26);

		//返回
		str = "back";

		CCMenuItem *back =CCMenuItemFont::create(str.c_str(),this,menu_selector(SettingScene::backCallback));

		//窗口布局
		CCMenu *menu = CCMenu::create(title1,soundItem,title2,bulletNumItem,back,NULL);
		menu->alignItemsInColumns(2,2,1);
		this->addChild(menu);

		back->setScale(0.8f);
		back->setPosition(ccp(back->getPosition().x,back->getPosition().y - 20));

		bRet = true;
	} while (0);

	return bRet;
}

cocos2d::CCScene* SettingScene::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		SettingScene *layer = SettingScene::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

void SettingScene::backCallback( cocos2d::CCObject *pSender )
{
	CCScene *scene=GameScene::createScene();
	CCDirector::sharedDirector()->replaceScene(CCTransitionFlipX::create(1.2f,scene));
}

void SettingScene::soundCallback( cocos2d::CCObject *pSender )
{
	g_isPlaySoundEffect=!g_isPlaySoundEffect;
}

void SettingScene::bulletNumCallback( cocos2d::CCObject *pSender )
{
	g_bulletNum+=50;

	if(g_bulletNum>150)
	{
		g_bulletNum=50;
	}
}
