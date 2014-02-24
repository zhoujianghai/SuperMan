#include "bullet.h"
#include "utils.h"


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
