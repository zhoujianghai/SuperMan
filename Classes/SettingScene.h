#ifndef _SETTING_SCENE_H_
#define _SETTING_SCENE_H_

#include "cocos2d.h"

#include "SimpleAudioEngine.h"

class SettingScene : public cocos2d::Layer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::Scene* scene();

	void backCallback(cocos2d::Object *pSender);
	void soundCallback(cocos2d::Object *pSender);
	void bulletNumCallback(cocos2d::Object *pSender);

	// implement the "static node()" method manually
	CREATE_FUNC(SettingScene);
};

#endif
