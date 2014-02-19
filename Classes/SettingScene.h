#ifndef _SETTING_SCENE_H_
#define _SETTING_SCENE_H_

#include "cocos2d.h"

#include "SimpleAudioEngine.h"

class SettingScene : public cocos2d::CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();

	void backCallback(cocos2d::CCObject *pSender);
	void soundCallback(cocos2d::CCObject *pSender);
	void bulletNumCallback(cocos2d::CCObject *pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(SettingScene);
};

#endif
