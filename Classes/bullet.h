#ifndef SUPER_BULLET_H_
#define SUPER_BULLET_H_

#include <cocos2d.h>

typedef enum
{
	DIR_UP = 0,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT
}BULLET_DIR;

class Bullet : public cocos2d::Sprite
{
public:
	Bullet();
	~Bullet();

	virtual bool init();

	CC_SYNTHESIZE_READONLY(int, _speed_x, _speed_x);
	CC_SYNTHESIZE_READONLY(int, _speed_y, _speed_y);

	CREATE_FUNC(Bullet);

};

#endif