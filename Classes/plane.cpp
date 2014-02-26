#include "plane.h"
#include "utils.h"

using namespace cocos2d;

Plane::Plane()
{

}

Plane::~Plane()
{

}

bool Plane::init()
{
	bool ret = false;
	do {

		CC_BREAK_IF(!this->initWithSpriteFrameName("hero_01.png"));

		auto self_anim = createAnimation("hero_%02d.png", 7, 10);
		this->runAction(RepeatForever::create(Animate::create(self_anim)));
		ret = true;
	}while(0);

	return ret;
}