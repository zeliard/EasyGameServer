#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::LayerColor
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	
	void spriteMoveFinished(cocos2d::CCNode * sender); 
	void gameLogic(float dt); 
	
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {}
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	void helloWorld() { CCLOG("HELLO~~~"); }
	void updatePeer(int id, float x, float y);
	void updateMe(float x, float y);
	void chatDraw(const std::string& from, const std::string& chat);


private:
	cocos2d::CCSprite* mPlayerSprite = nullptr;
	std::map<int, cocos2d::CCSprite*> mPeerMap;
};

#endif // __HELLOWORLD_SCENE_H__
