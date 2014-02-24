#include "AppDelegate.h"
#include "cocos2d.h"
#include "CCEventType.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

void cocos_android_app_init (struct android_app* app) {
    LOGD("cocos_android_app_init");
    AppDelegate *pAppDelegate = new AppDelegate();
}

void showAds(bool show)
{
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t, "com/alexzhou/superman/SuperManActivity", "changeAdShow", "(Z)V"))
	{
		jboolean jshow = show;
	    t.env->CallStaticVoidMethod(t.classID, t.methodID, jshow);
	}
}
