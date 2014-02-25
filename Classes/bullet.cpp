#include "bullet.h"
#include "utils.h"


// bullet shape vetex
		//{-0.32667953968048097, 7.0398719787597654},
		//{-6.305376052856445, 5.44439697265625},
		//{-7.983097839355469, -0.21791348457336424},
		//{-5.676230621337891, -4.831647872924805},
		//{-0.8527810096740722, -6.509369659423828},
		//{5.85810546875, -4.831647872924805},
		//{7.326111602783203, 0.8306625366210938},
		//{5.228959655761718, 5.654112243652344},



using namespace cocos2d;

Bullet::Bullet()
	:_speed_x(0),_speed_y(0)
{

}


Bullet::~Bullet()
{

}

bool Bullet::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF(!this->initWithSpriteFrameName("bullet_1.png"));

		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto origin = Director::getInstance()->getVisibleOrigin();

		const int direction = abs(rand() % 4);
		const int speedUnit = 4;
		float x = 0, y = 0;
		Size bullet_show_size = this->getDisplayFrame()->getRect().size;
		switch(direction) {
			case DIR_UP:
			{
				x = abs(rand()%(int)visibleSize.width);
				y = visibleSize.height - bullet_show_size.height;
				_speed_x = rand()%speedUnit;
				_speed_y = (abs(rand()%speedUnit + 1)) * (-1);
				break;
			}

			case DIR_DOWN:
			{
				x = abs(rand()%(int)visibleSize.width);
				y = bullet_show_size.height;
				_speed_x = rand()%speedUnit;
				_speed_y = abs(rand()%speedUnit+1);
				break;
			}

			case DIR_LEFT:
			{
				x = bullet_show_size.width;
				y = abs(rand()%(int)visibleSize.height);
				_speed_x = abs(rand()%speedUnit + 1);
				_speed_y = rand()%speedUnit;
				break;
			}

			case DIR_RIGHT:
			{
				x = visibleSize.width - bullet_show_size.width;
				y = abs(rand()%(int)visibleSize.height);
				_speed_x = abs(rand()%speedUnit + 1) * (-1);
				_speed_y = rand()%speedUnit;
				break;
			}
		}

		this->setPosition(Point(origin.x + x, origin.y + y));

		auto self_anim = createAnimation("bullet_%d.png", 4, 10);
		this->runAction(RepeatForever::create(Animate::create(self_anim)));

		ret = true;
	}while(0);

	return ret;
}
