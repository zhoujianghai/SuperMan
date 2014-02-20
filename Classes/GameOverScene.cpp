#include "GameOverScene.h"
#include "GameScene.h"
#include "global.h"
#include "tools.h"

using namespace cocos2d;
using namespace std;


bool GameOverScene::init()
{
	bool bRet = false;
	do 
	{
		Size size = Director::sharedDirector()->getWinSize();

		auto pLabel = LabelTTF::create("GameOver", "Thonburi", 30);
		CC_BREAK_IF(! pLabel);
		// Get window size and place the label upper. 		
		pLabel->setPosition(ccp(size.width / 2, size.height*0.9f));
		// Add the label to TitleScene layer as a child layer.
		this->addChild(pLabel, 1);

		std::string keepTime="you continue";
		std::string second="second";
		char szBuf[128];
		memset(szBuf,0,sizeof(szBuf));
		sprintf(szBuf," %d ",(int)g_gameTime);
		std::string titleStr3 = keepTime + szBuf + second;
		auto pLabel3=LabelTTF::create(titleStr3.c_str(), "Thonburi", 30);

		// Get window size and place the label upper. 		
		pLabel3->setPosition(ccp(size.width / 2, size.height*0.7f));
		// Add the label to TitleScene layer as a child layer.
		this->addChild(pLabel3, 1);


		auto pLabel4 = LabelTTF::create("you get: ", "Thonburi", 30);

		// Get window size and place the label upper. 		
		pLabel4->setPosition(ccp(size.width / 2, size.height*0.5f));
		// Add the label to TitleScene layer as a child layer.
		this->addChild(pLabel4, 1);

		std::string appraise = convertAppraise();
		auto pLabel5 = LabelTTF::create(appraise.c_str(), "Thonburi", 30);
		
		// Get window size and place the label upper. 		
		pLabel5->setPosition(ccp(size.width / 2, size.height*0.3f));
		// Add the label to TitleScene layer as a child layer.
		this->addChild(pLabel5, 1);

		auto pLabel2 = LabelTTF::create("return", "Thonburi", 30);

		// Get window size and place the label upper. 		
		pLabel2->setPosition(ccp(size.width / 2, size.height*0.1f));
		// Add the label to TitleScene layer as a child layer.
		this->addChild(pLabel2, 1);

		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(GameOverScene::onTouchesBegan, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		bRet = true;
	}while (0);

	return bRet;
}

Scene* GameOverScene::scene()
{
	Scene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = Scene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		GameOverScene *layer = GameOverScene::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);

	} while (0);

	// return the scene
	return scene;
}

void GameOverScene::onTouchesBegan(const vector<Touch*>& touches, Event *unused_event)
{
	Scene *scene=GameScene::createScene();
	Director::sharedDirector()->replaceScene(TransitionFade::create(1.2f,scene));
}

std::string GameOverScene::convertAppraise()
{
	std::string appraise;

	switch((int)g_gameTime)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		appraise="11";
		break;
	case 7:
		appraise="22";
		break;
	case 8:
		appraise="33";
		break;
	case 9:
		appraise="44";
		break;
	case 10:
		appraise="55";
		break;
	case 11:
		appraise="66";
		break;
	case 12:
		appraise="77";
		break;
	case 13:
		appraise="88";
		break;
	case 14:
		appraise="99";
		break;
	case 15:
		appraise="100";
		break;
	case 16:
		appraise="101";
		break;
	case 17:
		appraise="102";
		break;
	case 18:
		appraise="103";
		break;
	case 19:
		appraise="104";
		break;
	case 20:
	default:
		appraise="105";
	}

	return appraise;
}
