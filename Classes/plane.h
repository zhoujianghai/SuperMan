#ifndef SUPERMAN_PLANE_H_
#define SUPERMAN_PLANE_H_

#include <cocos2d.h>


class Plane : public cocos2d::Sprite
{
public:
	Plane();
	virtual ~Plane();

	virtual bool init();

	CREATE_FUNC(Plane);
};

#endif