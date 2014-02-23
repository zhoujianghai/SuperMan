#include "plane.h"

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

		//CC_BREAK_IF(!this->initWithSpriteFrameName("plane.png"));
		CC_BREAK_IF(!this->initWithFile("plane_small.png"));
		ret = true;
	}while(0);

	return ret;
}