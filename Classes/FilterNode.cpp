#include "FilterNode.h"
#include "cocos-ext.h"
static const char* shader_res[2] = {
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif \n"
	"uniform sampler2D u_texture; \n"
	"varying vec2 v_texCoord;\n"
	"uniform float ratio; \n"
	"uniform float hsize;\n"
	"void main(void)  \n"
	"{  \n"
		"vec4 sum = vec4(0.0);  \n"
		"float blurSize = ratio / hsize; \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x - 4.0 * blurSize, v_texCoord.y)) *  1.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x - 3.0 * blurSize, v_texCoord.y)) *  2.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x - 2.0 * blurSize, v_texCoord.y)) *  3.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x -   blurSize, v_texCoord.y)) * 10.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x , v_texCoord.y)) * 15.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x +  blurSize, v_texCoord.y)) * 10.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x + 2.0 * blurSize, v_texCoord.y)) *  3.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x + 3.0 * blurSize, v_texCoord.y)) *  2.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x + 4.0 * blurSize, v_texCoord.y)) *  1.0/47.0;  \n"
		"gl_FragColor = sum;  \n"
	"}\n",
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"
	"uniform sampler2D u_texture;\n"
	"varying vec2 v_texCoord;\n"
	"uniform float ratio; \n"
	"uniform float vsize;\n"
	"void main(void)  \n"
	"{   \n"
		"vec4 sum = vec4(0.0);  \n"
		"float blurSize = ratio / vsize;\n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y - 4.0 * blurSize)) *  1.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y - 3.0 * blurSize)) *  2.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y - 2.0 * blurSize)) *  3.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y -       blurSize)) * 10.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y  )) * 15.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y +       blurSize)) * 10.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y + 2.0 * blurSize)) *  3.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y + 3.0 * blurSize)) *  2.0/47.0;  \n"
		"sum += texture2D(u_texture, vec2(v_texCoord.x, v_texCoord.y + 4.0 * blurSize)) *  1.0/47.0;  \n"
		"gl_FragColor = sum;  \n"
	"}\n"
};

/*********************| FilterNode Classes |*********************/
FilterNode::FilterNode():_srcNode(NULL)
{ }

FilterNode::~FilterNode()
{ }

CCGLProgram* FilterNode::commonPrograme(const char* key, const GLchar* src)
{
	CCGLProgram* pp = CCShaderCache::sharedShaderCache()->programForKey(key);
	if (!pp) {
		pp = new CCGLProgram();
		CCShaderCache::sharedShaderCache()->addProgram(pp, key);
		pp->initWithVertexShaderByteArray(ccPositionTextureColor_vert, src);
		pp->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		pp->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
		pp->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		pp->link();
		pp->updateUniforms();
	}
	return pp;
}

FilterNode* FilterNode::create(CCNode* src, const CCSize& size)
{
	FilterNode* ret = new FilterNode();
	if (ret && ret->init(src, size)) {
		ret->autorelease();
		return ret;
	} else {
		CC_SAFE_RELEASE(ret);
		return NULL;
	}
}

bool FilterNode::init(CCNode* src, const CCSize& size)
{
	bool bRet = false;
	do {
		if (_srcNode) {
			CCLog("init can only be called once!");
		}
		CC_BREAK_IF(!CCRenderTexture::initWithWidthAndHeight(size.width, size.height,
								     kCCTexture2DPixelFormat_RGBA8888));
		_srcNode = src;
		if (_srcNode)
			_pos = _srcNode->getPosition();
		else
			_pos = CCPointZero;
		bRet = true;
	}while(0);
	return bRet;
}

void FilterNode::capture()
{
	if (!_srcNode) return;
	CCPoint pos = _srcNode->getPosition();
	CCPoint ar = _srcNode->getAnchorPoint();
	_pos = ccpSub(pos, _pos);
	_srcNode->setPosition(ar.x * _srcNode->getContentSize().width, ar.y * _srcNode->getContentSize().height);
	this->beginWithClear(0, 0, 0, 0);
	_srcNode->visit();
	this->end();
	_srcNode->setPosition(pos);
	this->getSprite()->setPosition(ccpAdd(this->getSprite()->getPosition(), _pos));
	_pos = pos;
}

/*********************| GuassianBlur class |*********************/
extern GLchar*	 hblurSrc = NULL;
extern GLchar*	 vblurSrc = NULL;
static GaussianBlur* _scSizeBlurNode = NULL;

GaussianBlur::GaussianBlur():f1(NULL), f2(NULL), _showing(false)
{  }

GaussianBlur::~GaussianBlur()
{
	CC_SAFE_RELEASE_NULL(f1);
	CC_SAFE_RELEASE_NULL(f2);
}

CCGLProgram*GaussianBlur::blurProgram(bool isV, GLfloat ratio, GLfloat hvsize)
{
	CCGLProgram* p = FilterNode::commonPrograme(isV ? "vBlur" : "hBlur", shader_res[isV ? : 1 : 0]);
	setBlurData(p, isV, ratio, hvsize);
	return p;
}

void GaussianBlur::setBlurData(CCGLProgram* p, bool isV, GLfloat ratio, GLfloat hvsize)
{
	if (p) {
		p->use();
		GLint blurSLoc = glGetUniformLocation(p->getProgram(), "ratio");
		p->setUniformLocationWith1f(blurSLoc, ratio);
		blurSLoc = glGetUniformLocation(p->getProgram(), isV ? "vsize" : "hsize");
		p->setUniformLocationWith1f(blurSLoc, hvsize);
	}
}

GaussianBlur*GaussianBlur::create(CCNode* src, const CCSize& size)
{
	GaussianBlur* ret = new GaussianBlur();
	if (ret && ret->init(src, size)) {
		ret->autorelease();
		return ret;
	} else {
		CC_SAFE_DELETE(ret);
		return NULL;
	}
}

GaussianBlur*GaussianBlur::screenBlurNodeInstance()
{
	if (_scSizeBlurNode)
		return _scSizeBlurNode;
	_scSizeBlurNode = new GaussianBlur();
	_scSizeBlurNode->retain();
	_scSizeBlurNode->init(NULL, CCDirector::sharedDirector()->getVisibleSize(), true);
	return _scSizeBlurNode;
}

GaussianBlur* GaussianBlur::takeScreenCapture()
{
	GaussianBlur* n = screenBlurNodeInstance();
	CCNode* s = CCDirector::sharedDirector()->getRunningScene();
	if (s) {
		CCPoint a = s->getAnchorPoint();
		s->setAnchorPoint(CCPointZero);
		n->reset(s);
		s->setAnchorPoint(a);
		return n;
	}
	return NULL;
}

bool GaussianBlur::init(CCNode* src, const CCSize& size, bool reused)
{
	bool bRet = false;
	do {
		CC_BREAK_IF(!CCNode::init());
		f1 = FilterNode::create(src, size);
		CC_BREAK_IF(!f1);
		f1->retain();
		CCGLProgram* s = FilterNode::commonPrograme("hBlur", shader_res[0]);
		f1->filter(s);
		setBlurSize(size, 1);
		f1->capture();
		s->release();

		f2 = FilterNode::create(f1->getSprite(), size);
		CC_BREAK_IF(!f2);
		s = FilterNode::commonPrograme("vBlur", shader_res[1]);
		f2->filter(s);
		setBlurSize(size, 2);
		f2->retain();
		f2->capture();
		s->release();

		this->setContentSize(size);
		this->setAnchorPoint(CCPointZero);
		this->setPosition(CCPointZero);
		f2->setPosition(size.width /2, size.height/2);
		this->addChild(f2);

		bRet = true;
	}while(0);
	return bRet;
}

void GaussianBlur::setBlurSize(const CCSize& size, const int which , GLfloat ratio)
{
	if (!which || 1 == which) {
		setBlurData(f1->getSprite()->getShaderProgram(), false, ratio, size.width);
	}
	if (!which || 2 == which) {
		setBlurData(f2->getSprite()->getShaderProgram(), true, ratio, size.height);
	}
}

bool GaussianBlur::reset(CCNode* src)
{
	f1->changeNode(src);
	f2->changeNode(f1->getSprite());
}

void GaussianBlur::cleanFromWorld()
{
	realtime(false);
	this->removeFromParent();
	this->removeAllChildren();
	this->addChild(f2);
	_showing = false;
}

void GaussianBlur::show()
{
	if (_showing) return;
	CCScene* s =CCDirector::sharedDirector()->getRunningScene();
	if (s) {
		_showing = true;
		s->addChild(this);
	}
}

void GaussianBlur::capture()
{
	f1->capture();
	f2->capture();
}

