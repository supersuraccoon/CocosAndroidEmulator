#include "AppDelegate.h"

#include "jsb_cocos2dx_auto.hpp"
#include "cocosbuilder/js_bindings_ccbreader.h"

USING_NS_CC;

#include "GameInfo.pb.h"
#include "ScriptingCore.h"

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    ScriptEngineManager::destroyInstance();
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
        glview = cocos2d::GLViewImpl::create("CocosProtobuf");
#else
        glview = cocos2d::GLViewImpl::createWithRect("CocosProtobuf", Rect(0,0,960,640));
#endif
        director->setOpenGLView(glview);
}

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    ScriptingCore* sc = ScriptingCore::getInstance();
    sc->addRegisterCallback(register_all_cocos2dx);
    sc->addRegisterCallback(register_cocos2dx_js_core);
    
    sc->start();    
    sc->runScript("script/jsb_boot.js");
    
    ScriptEngineProtocol *engine = ScriptingCore::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    ScriptingCore::getInstance()->runScript("main.js");
    
    
    // test send protobuf message from C++
    JSObject* pGlobalObject = ScriptingCore::getInstance()->getGlobalObject();
    JSContext* pGlobalContext = ScriptingCore::getInstance()->getGlobalContext();

    // Create a GameInfo object first
    game::info::GameInfo gameInfo;
    
    // add RoleInfo object to repeated field
    game::info::RoleInfo *pRoleInfo1 = gameInfo.add_roleinfo();
    pRoleInfo1->set_name("cpp_name1");
    pRoleInfo1->set_type(game::enumeration::FIGHTER);
    
    // add another RoleInfo object to repeated field
    game::info::RoleInfo *pRoleInfo2 = gameInfo.add_roleinfo();
    pRoleInfo2->set_name("cpp_name2");
    pRoleInfo2->set_type(game::enumeration::BOWMAN);
    
    // add ItemInfo object to repeated field
    game::info::ItemInfo *pItemInfo1 = gameInfo.add_iteminfo();
    pItemInfo1->set_name("cpp_item1");
    pItemInfo1->set_price(100);
    
    // add another ItemInfo object to repeated field
    game::info::ItemInfo *pItemInfo2 = gameInfo.add_iteminfo();
    pItemInfo2->set_name("cpp_item2");
    pItemInfo2->set_price(200);
    
    std::string gameInfoString;
    gameInfo.SerializeToString(&gameInfoString);
    
    // need this before JS_NewArrayBuffer
    JSAutoCompartment ac(pGlobalContext, pGlobalObject);
    int length = gameInfoString.length();
    JSObject* pMessageJSObject = JS_NewArrayBuffer(pGlobalContext, length);
    uint8_t* pMessageData = JS_GetArrayBufferData(pMessageJSObject);
    memcpy((void*)pMessageData, (const void*)gameInfoString.c_str(), gameInfoString.length());
    
    JS::RootedValue ret(pGlobalContext);
    jsval arg = OBJECT_TO_JSVAL(pMessageJSObject);
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(pGlobalObject), "test_protobuf", 1, &arg, &ret);
    
    //
    game::info::GameInfo* pGameInfo = new game::info::GameInfo;
    JSObject* pRetJSObject = ret.toObjectOrNull();
    if(pRetJSObject && JS_IsArrayBufferObject(pRetJSObject)) {
        uint8_t* pDataBuffer = nullptr;
        int dataBufferCount = 0;
        pDataBuffer = JS_GetArrayBufferData(pRetJSObject);
        dataBufferCount = JS_GetArrayBufferByteLength(pRetJSObject);
        pGameInfo->ParseFromArray(pDataBuffer, dataBufferCount);
        
        // RoleInfo Array
        for (int i = 0; i < pGameInfo->roleinfo_size(); i++) {
            // role info
            game::info::RoleInfo roleInfo = pGameInfo->roleinfo(i);
            CCLOG("roleInfo: \n%s\n", roleInfo.Utf8DebugString().c_str());
        }
        
        // ItemInfo Array
        for (int i = 0; i < pGameInfo->iteminfo_size(); i++) {
            // role info
            game::info::ItemInfo itemInfo = pGameInfo->iteminfo(i);
            CCLOG("itemInfo: \n%s\n", itemInfo.Utf8DebugString().c_str());
        }
    }
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    auto director = Director::getInstance();
    director->stopAnimation();
    director->getEventDispatcher()->dispatchCustomEvent("game_on_hide");
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    auto director = Director::getInstance();
    director->startAnimation();
    director->getEventDispatcher()->dispatchCustomEvent("game_on_show");
}
