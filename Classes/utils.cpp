#include "utils.h"

using namespace cocos2d;

Animation* createAnimation(const char* path, int frame_count, int fps)
{
	Array *frames = Array::createWithCapacity(frame_count);
	for(int i = 1; i <= frame_count; ++ i) {
		const char* img_name = String::createWithFormat(path, i)->getCString();
		//log("img_name=%s", img_name);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(img_name);
		frames->addObject(frame);
	}
	return Animation::createWithSpriteFrames(frames, 1.0 / fps);
}