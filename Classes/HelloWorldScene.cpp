#include "HelloWorldScene.h"
#include "cocos-ext.h"
#include "FilterNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define RTBTNTAG  9
#define SBTNTAG  8
CCScene* HelloWorld::scene()
{
	CCScene *scene = CCScene::create();
	HelloWorld *layer = HelloWorld::create();
	scene->addChild(layer);
	return scene;
}

HelloWorld::~HelloWorld()
{ }

bool HelloWorld::init()
{
	if ( !CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)) ) {
		return false;
	}
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	CCSprite* bg = CCSprite::create("bg.png");
	bg->setPosition(ccp(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(bg, 0, 0);
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
	                                      "CloseNormal.png",
	                                      "CloseSelected.png",
	                                      this,
	                                      menu_selector(HelloWorld::menuCloseCallback));

	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
	                            origin.y + pCloseItem->getContentSize().height/2));
	CCMenuItemFont* sBtn = CCMenuItemFont::create("", this, menu_selector(HelloWorld::btnCB));
	sBtn->setTag(SBTNTAG);
	sBtn->setAnchorPoint(CCPointZero);
	sBtn->setPosition(50, 200);
	sBtn->setColor(ccRED);
	CCMenuItemFont* rbtn = CCMenuItemFont::create("", this, menu_selector(HelloWorld::btnCB));
	rbtn->setTag(RTBTNTAG);
	rbtn->setAnchorPoint(CCPointZero);
	rbtn->setPosition(50, 300);
	rbtn->setColor(ccGREEN);

	// create menu, it's an autorelease object
	pMenu = CCMenu::create(pCloseItem, sBtn, rbtn, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	// show filter node
	this->setAnchorPoint(CCPointZero);
	fnode = GaussianBlur::screenBlurNodeInstance();
	fnode->reset(this);
	this->addChild(fnode);

	updateBtn(SBTNTAG);
	updateBtn(RTBTNTAG);

	return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif
}

void HelloWorld::updateBtn(const int tag)
{
	CCMenuItemFont* b = (CCMenuItemFont*)pMenu->getChildByTag(tag);
	if (tag == RTBTNTAG) {
		b->setString((fnode && fnode->isRealTime()) ? "stop realtime upadte" : "update in realtime");
	} else if (tag == SBTNTAG) {
		b->setString(fnode ? "hide filter" : "show filter");
	}
}

void HelloWorld::btnCB(CCObject* pSender)
{
	const int tag = ((CCNode*)pSender)->getTag();
	if (tag == RTBTNTAG) {
		if (fnode && fnode->isVisible()) {
			fnode->realtime(!fnode->isRealTime());
		}
	} else if (tag == SBTNTAG) {
		CCSprite* bg = (CCSprite*)this->getChildByTag(0);
		if (fnode) {
			// remove from view (中文:调用此方法来删除显示在模糊节点上的节点, 并清除自己)
			fnode->cleanFromWorld();
			fnode = NULL;
			bg->stopAllActions();
		} else {
			fnode = GaussianBlur::screenBlurNodeInstance();
			CCSprite* a = CCSprite::create("a.png");
			a->setPosition(ccp(400, 240));
			fnode->addChild(a);
			fnode->reset(this);
			fnode->show();
			// or you may use this like code(中文:你也许更喜欢用类似的显示方式)
			//CCDirector::sharedDirector()->getRunningScene()->addChild(fnode);
			
			// run a action to show realtime update blur effect(中文:显示一个动画，来凸显实时模糊的功能)
			bg->setPosition(ccpSub(bg->getPosition(), ccp(200, 0)));
			bg->runAction(CCRepeatForever::create(
					      CCSequence::createWithTwoActions(
					      CCMoveBy::create(1.0f, ccp(400, 0)),
					      CCMoveBy::create(1.0f, ccp(-400, 0))
						      )));
		}
	}
	updateBtn(RTBTNTAG);
	updateBtn(SBTNTAG);
}

