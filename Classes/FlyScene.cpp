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
		CC_BREAK_IF(! CCLayer::init());

		CCSize s = CCDirector::sharedDirector()->getWinSize();	

		auto joystick1 = CCSprite::create("joystick1.png");
		joystick1->setOpacity(191);
		joystick1->setAnchorPoint(ccp(0,0));
		joystick1->setPosition(ccp(0,0));
		radius=joystick1->getContentSize().width/2;
		centre=ccp(radius,radius);
		this->addChild(joystick1,1);

		joystick = CCSprite::create("joystick2.png");
		joystick->setPosition(ccp(centre.x,centre.y));
		this->addChild(joystick,2);

		//返回
		CCMenuItemFont::setFontSize(22);
		CCMenuItemFont::setFontName("American Typewriter");
		std::string str = "";

		str="back";

		auto systemMenu = CCMenuItemFont::create(str.c_str(), CC_CALLBACK_1(FlyScene::menuReturnCallback, this));

		auto mn = CCMenu::create(systemMenu, NULL);
		mn->setPosition(ccp(0,0));

		systemMenu->setAnchorPoint(ccp(1,0));
		systemMenu->setPosition(ccp(s.width,0));

		this->addChild(mn,1);

		//飞机
		CCTexture2D *texture=CCTextureCache::sharedTextureCache()->addImage("plane.png");
		plane = CCSprite::createWithTexture(texture);
		plane->setPosition(ccp(s.width/2,s.height/2));
		this->addChild(plane,0);

		//开启触摸
		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(FlyScene::onTouchesBegan, this);
		listener->onTouchesMoved = CC_CALLBACK_2(FlyScene::onTouchesMoved, this);
		listener->onTouchesEnded = CC_CALLBACK_2(FlyScene::onTouchesEnded, this); 
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		//开启逻辑
		this->schedule(schedule_selector(FlyScene::flying));
		this->schedule(schedule_selector(FlyScene::createBullet));
		this->schedule(schedule_selector(FlyScene::checkBullet));
		this->schedule(SEL_SCHEDULE(&FlyScene::saveTime),1);
		//清零数据
		isFlying=false;
		speedX=speedY=0;
		g_gameTime=0;

		bRet = true;
	}while(0);
	return bRet;
}

cocos2d::CCScene* FlyScene::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
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

void FlyScene::menuReturnCallback( CCObject *pSender )
{
	CCScene * scene = GameScene::createScene();
	CCDirector::sharedDirector()->replaceScene(CCTransitionFlipX::create(1.2f,scene));
}

void FlyScene::onTouchesBegan(const vector<Touch*>& touches, Event *unused_event)
{
	vector<Touch*>::const_iterator touchIter = touches.begin();
	Touch *pTouch = (Touch*)(*touchIter);
	Point dest = pTouch->getLocation();
	CCRect rect=joystick->boundingBox();
	if(rect.containsPoint(dest))
	{
		isFlying=true;
	}
}

void FlyScene::onTouchesMoved(const vector<Touch*>& touches, Event *unused_event)
{
	vector<Touch*>::const_iterator touchIter = touches.begin();
	Touch *pTouch = (Touch*)(*touchIter);
	Point dest = pTouch->getLocation();

	bool inRange = pow(centre.x - dest.x,2)+pow(centre.y-dest.y,2)<pow(radius,2);

	if(isFlying&&inRange)
	{
		CCPoint position=plane->getPosition();
		joystick->setPosition(dest);

		float radius1=radius*2/6;//十字键中心区的内切圆半径
		float side=radius*2/3;//九宫格中一个格子的边长

		//我们使用海伦公式来计算面积，进而判断十字键中心区的精确方向
		//向上
		if(triangleContainPoint(centre.x,centre.y,centre.x-radius1,centre.y+radius1,centre.x+radius1,centre.y+radius1,dest.x,dest.y)
			||Rect(centre.x-radius1, centre.y+radius1, side, side).containsPoint(dest))
		{
			speedX=0;
			speedY=1;
		}
		//向下
		else if(triangleContainPoint(centre.x,centre.y,centre.x-radius1,centre.y-radius1,centre.x+radius1,centre.y-radius1,dest.x,dest.y)
			||Rect(centre.x-radius1,centre.y-radius1-side,side,side).containsPoint(dest))
		{
			speedX=0;
			speedY=-1;
		}
		//向左
		else if(triangleContainPoint(centre.x,centre.y,centre.x-radius1,centre.y+radius1,centre.x-radius1,centre.y-radius1,dest.x,dest.y)
			||Rect(centre.x-radius1-side,centre.y-radius1,side,side).containsPoint(dest))
		{
			speedX=-1;
			speedY=0;
		}
		//向右
		else if (triangleContainPoint(centre.x,centre.y,centre.x+radius1,centre.y+radius1,centre.x+radius1,centre.y-radius1,dest.x,dest.y)
			||Rect(centre.x+radius1+side,centre.y-radius1,side,side).containsPoint(dest))
		{
			speedX=1;
			speedY=0;
		}
		//右上
		else if(dest.x-centre.x>0&&dest.y-centre.y>0)
		{
			speedX=0.7f;
			speedY=0.7f;
		}
		//左上
		else if (dest.x-centre.x<0&&dest.y-centre.y>0)
		{
			speedX=-0.7f;
			speedY=0.7f;
		}
		//左下
		else if (dest.x-centre.x<0&&dest.y-centre.y<0)
		{
			speedX=-0.7f;
			speedY=-0.7f;
		}
		//右下
		else if (dest.x-centre.x>0&&dest.y-centre.y<0)
		{
			speedX=0.7f;
			speedY=-0.7f;
		}
	}
}

void FlyScene::onTouchesEnded(const vector<Touch*>& touches, Event *unused_event)
{
	isFlying=false;
	joystick->setPosition(centre);
	speedX=speedY=0;
}

void FlyScene::flying(float dt )
{
	if (isFlying&&(speedX!=0||speedY!=0))
	{
		CCPoint position=ccp(plane->getPosition().x+speedX,plane->getPosition().y+speedY);

		CCSize size=CCDirector::sharedDirector()->getWinSize();
		CCRect rect=CCRectMake(0,0,size.width,size.height);

		if(rect.containsPoint(position))
		{
			plane->setPosition(position);
		}
	}

}

void FlyScene::createBullet( float dt )
{
	if (bullets.size()>g_bulletNum)
	{
		return;
	}

	CCTexture2D *texture=CCTextureCache::sharedTextureCache()->addImage("bullet.png");
	CCSprite *bullet=CCSprite::createWithTexture(texture);
	this->addChild(bullet,0);
	bullets.push_back(bullet);

	float x,y;
	int speedX,speedY;
	CCSize size=CCDirector::sharedDirector()->getWinSize();

	int entrance=abs(rand()%4);
	switch(entrance)
	{
	case 0://上侧飞入
		x=abs(rand()%(int)size.width);
		y=size.height-bullet->boundingBox().size.height;
		speedX=rand()%3;
		speedY=(abs(rand()%3+1))*-1;
		break;
	case 1://下侧飞入
		x=abs(rand()%(int)size.width);
		y=bullet->boundingBox().size.height;
		speedX=rand()%3;
		speedY=abs(rand()%3+1);
		break;
	case 2://左侧飞入
		x=bullet->boundingBox().size.width;
		y=abs(rand()%(int)size.height);
		speedX=abs(rand()%3+1);
		speedY=rand()%3;
		break;
	case 3://右侧飞入
		x=size.width-bullet->boundingBox().size.width;
		y=abs(rand()%(int)size.height);
		speedX=abs(rand()%3+1)*-1;
		speedY=rand()%3;
		break;
	}

	CCActionInterval *action=CCMoveBy::create(0.1f,ccp(speedX,speedY));

	bullet->setPosition(ccp(x,y));
	bullet->runAction(CCRepeatForever::create(action));
}

void FlyScene::checkBullet(float dt)
{
	if (!plane->isVisible())
	{
		return;
	}

	CCSize size=CCDirector::sharedDirector()->getWinSize();
	CCRect screen=CCRectMake(0,0,size.width,size.height);
	CCRect planeBox=plane->boundingBox();

	for (std::vector<cocos2d::CCSprite*>::iterator bitr=bullets.begin(); bitr != bullets.end();)
	{
		CCSprite *bullet =*bitr;
		CCPoint position=bullet->getPosition();
		CCRect bulletBox=bullet->boundingBox();

		if (planeBox.intersectsRect(bulletBox))
		{
			this->removeChild(bullet, true);
			bitr=bullets.erase(bitr);

			plane->setVisible(false);

			CCTexture2D *texture=CCTextureCache::sharedTextureCache()->addImage("explosion.png");
			CCAnimation *animation = CCAnimation::create();
			animation->setDelayPerUnit(0.2f);
			for(int i=0;i<4;i++)
				animation->addSpriteFrameWithTexture(texture,CCRectMake(i*32,0,32,32));
			CCAnimate *animate=CCAnimate::create(animation);
			CCFiniteTimeAction *animateOver=CCCallFunc::create(this,callfunc_selector(FlyScene::explosionEndDid));
			CCFiniteTimeAction *seq=CCSequence::create(animate,animateOver,NULL);

			explosion=CCSprite::createWithTexture(texture,CCRectMake(0,0,32,32));
			this->addChild(explosion);
			explosion->setPosition(plane->getPosition());
			explosion->runAction(seq);			

			if(g_isPlaySoundEffect)
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("explosion.wav");

			break;
		}
		else if (!screen.containsPoint(position))
		{//离开屏幕就删除
			this->removeChild(bullet, true);
			bitr=bullets.erase(bitr);
		}
		else
		{
			bitr++;
		}
	}
}

void FlyScene::saveTime( float dt )
{
	g_gameTime+=dt;
}

void FlyScene::explosionEndDid()
{
	explosion->setVisible(false);
	CCScene *scene=GameOverScene::scene();
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.2f,scene));
}
