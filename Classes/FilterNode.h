#ifndef FILETERSPRITE_H
#define FILETERSPRITE_H

/**
 * Desc: 特效node
 * Auth: 张宇飞
 * Date: 2014-08-19    
 */

#include "cocos2d.h"
USING_NS_CC;

class FilterProto
{
public:
	FilterProto():_rt(false) {}
	virtual void			capture() = 0;
	virtual void			updateCapture(bool b) = 0;
	void				realtime(bool b) {
		if (b != _rt) {
			_rt = b;
			updateCapture(b);
		}
	}
	inline bool			isRealTime() {return _rt;}
private:
	bool				_rt;
};

#define UPCAP_FUNC(_cls_, _freshfunc_) \
	void updateCapture(bool b) { \
		if (b) \
			this->schedule(schedule_selector(_cls_::_freshfunc_)); \
		else \
			this->unschedule(schedule_selector(_cls_::_freshfunc_));\
	}

/**
 * @brief The FilterNode class
 */
class FilterNode : public CCRenderTexture, public FilterProto
{
public:
	FilterNode();
	~FilterNode();
public:
	static CCGLProgram*		commonPrograme(const GLchar* src);
	static FilterNode*		create(CCNode* src, const CCSize& size);
	inline static FilterNode*	create(CCNode* src) {
		if (src) return create(src, src->getContentSize());
		return NULL;
	}
public:
	bool 				init(CCNode* src, const CCSize& size);
	/* set filter */
	inline void 			filter(CCGLProgram* shader) {
		this->getSprite()->setShaderProgram(shader);
	}
	inline void			changeNode(CCNode* src) {_srcNode = src; capture();}

	/* take a capture means do draw elements once 快照 */
	void				capture();
	UPCAP_FUNC(FilterNode, freshCapture)
private:
	void				freshCapture(float dt) { capture();}
private:
	CCNode*				_srcNode;
	CCPoint				_pos;
};

/**
 * @brief The GaussianBlur class use two FilterNode, one with horizontal blur and 
 * send result to next FilerNode with vertical blur .Then sencond result will show
 * as Gaussian blur effect
 * 中文:这个类用了两个FilterNode来实现高斯模糊。第一个用横向的模糊，结果作为第二个的
 * 输入进行纵向模糊，得到的结果就是需要的了。
 */
class GaussianBlur : public CCNode, public FilterProto
{
public:
	GaussianBlur();
	~GaussianBlur();
public:
	/* call this at global init time */
	static bool			do_ready(); 
	/* call this at global end time */
	static void			do_free();

	static GaussianBlur*		create(CCNode* src, const CCSize& size);
	inline static GaussianBlur*	create(CCNode* src) {
		if (src) return create(src, src->getContentSize());
		return NULL;
	}
	/**
	 * @brief screenBlurNode get a blur node instance with screen size(中文:获取一个屏幕大小的模糊单例)
	 */
	static GaussianBlur*		screenBlurNodeInstance();
public:
	bool 				init(CCNode* src, const CCSize& size, bool reused = false);
	/**
	 * @brief setBlurSize 设置模糊程度
	 * @param size 模糊范围
	 * @param which 0 all , 1 horizontal blur, 2 vertical blur(中文:0横纵向同时模糊，1横向模糊设定，2纵向模糊设定）
	 * @param ratio 模糊半径
	 */
	void				setBlurSize(const CCSize& size, const int which = 0, GLfloat ratio = 3.0);
	bool				reset(CCNode* src);
	void				cleanFromWorld();
	/* @brief show try use runningScene()->addChild to show this(中文:尝试用getRunningScene()->addChild来显示当前节点 */
	void				show();
	void				capture();
	UPCAP_FUNC(GaussianBlur, freshCapture)
private:
	void				freshCapture(float dt) { capture();}
private:
	FilterNode			*f1, *f2;
	bool				_showing;
};

#endif // FILETERSPRITE_H
