#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    AppDelegate app;
    EGLView eglView;
<<<<<<< HEAD
    eglView.init("TestCPP", 960, 640);
=======
    eglView.init("TestCPP", 360, 640);
>>>>>>> ee99815bdbb1bfc560ea0a7fdc5e9667dd5f5ceb
    return Application::getInstance()->run();
}
