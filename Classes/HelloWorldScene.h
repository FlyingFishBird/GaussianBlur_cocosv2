#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
class GaussianBlur;
class HelloWorld : public cocos2d::CCLayerColor
{
public:
	virtual bool init();
	static cocos2d::CCScene* scene();
	CREATE_FUNC(HelloWorld);
	~HelloWorld();
private:
	void btnCB(CCObject* pSender);
	void menuCloseCallback(CCObject* pSender);
	void updateBtn(const int tag);
private:
	cocos2d::CCPoint touchPos;
	cocos2d::CCMenu* pMenu;
	GaussianBlur* fnode;
};

#endif // __HELLOWORLD_SCENE_H__
