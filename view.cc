#define linux
#define NDEBUG
#include "view.h"
#include "SkData.h"
#include "SkCanvas.h"
#include "SkDevice.h"
#include "SkGraphics.h"
#include "SkString.h"
#include "SkTouchGesture.h"
#include "SkWindow.h"

#include "SkTypeface.h"

//#include "gm.h"
#include <memory>


#include "gl/GrGLInterface.h"
#include "gl/GrGLUtil.h"
#include "GrRenderTarget.h"
#include "GrContext.h"
#include "SkGpuDevice.h"

static const char gUpdateWindowTitleEvtName[] = "SkUpdateWindowTitle";

class ClvWindow : public SkOSWindow {
	void* hwnd;
public:
	enum DeviceType {
		kRaster_DeviceType,
		kPicture_DeviceType,
#if SK_SUPPORT_GPU
		kGPU_DeviceType,
#if SK_ANGLE
		kANGLE_DeviceType,
#endif // SK_ANGLE
		kNullGPU_DeviceType,
#endif // SK_SUPPORT_GPU
		kDeviceTypeCnt
	};

	ClvWindow(void* hwnd);//:SkOSWindow{nullptr};//{}
	virtual ~ClvWindow(){detach();};

	virtual SkCanvas* createCanvas() override;

	virtual void draw(SkCanvas* canvas) override;

	void setDeviceType(DeviceType type);
	DeviceType getDeviceType() const { return fDeviceType; }

protected:
	virtual bool onHandleKey(SkKey key) override;
	virtual void onDraw(SkCanvas* canvas) override;
	virtual void onSizeChange() override;

	virtual bool onEvent(const SkEvent& evt) override;
	virtual bool onClick(Click* click) override;
private:
	DeviceType fDeviceType;

	//SkTouchGesture fGesture;
	//SkScalar fZoomLevel;
	//SkScalar fZoomScale;

	GrContext*              fCurContext;
	const GrGLInterface*    fCurIntf;
	GrRenderTarget*         fCurRenderTarget;
	SkTypeface*		fTypeface;
	int fMSAASampleCount;

	typedef SkOSWindow INHERITED;
};

class ClvView : public SkView {
public:
	ClvView():fBGColor(SK_ColorWHITE){
		setVisibleP(true);
		setClipToBounds(false);
	}

	virtual ~ClvView(){}

protected:
	void onDrawBackground(SkCanvas*);
	//virtual void draw(SkCanvas*) override;
	virtual void onDraw(SkCanvas*) override;

	SkColor fBGColor;
private:
	typedef SkView INHERITED;
};

#include <iostream>

using namespace std;
static void inline postEventToSink(SkEvent* evt, SkEventSink* sink) {
    evt->setTargetID(sink->getSinkID())->post();
}

ClvWindow::ClvWindow(void* hwnd):INHERITED{hwnd},hwnd(hwnd){
	cout<<__FUNCTION__<<endl;
	fTypeface = SkTypeface::CreateFromTypeface(NULL, SkTypeface::kBold);
	//setConfig(SkBitmap::kRGB_565_Config);
	setConfig(SkBitmap::kARGB_8888_Config);
	setVisibleP(true);
	setClipToBounds(false);
	//attachChildToFront(new ClvView{})->unref();

	fMSAASampleCount=0;
	AttachmentInfo attachmentInfo;
	bool result = attach(kNativeGL_BackEndType, fMSAASampleCount, &attachmentInfo);
	if(!result)
		cerr<<"3D Attach err"<<endl;

	fCurIntf = GrGLCreateNativeInterface();
	fCurContext = GrContext::Create(kOpenGL_GrBackend, (GrBackendContext) fCurIntf);
	if (NULL == fCurContext || NULL == fCurIntf) {
		cerr<<"GrContext Create err"<<endl;
		detach();
	}

	//attach(kNativeGL_BackEndType, fMSAASampleCount, &attachmentInfo);
	GrBackendRenderTargetDesc desc;
	desc.fWidth = SkScalarRound(width());
	desc.fHeight = SkScalarRound(height());

	desc.fConfig = kSkia8888_GrPixelConfig;
	desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
	desc.fSampleCnt = attachmentInfo.fSampleCount;
	desc.fStencilBits = attachmentInfo.fStencilBits;
	GrGLint FBid;
	GR_GL_GetIntegerv(fCurIntf, GR_GL_FRAMEBUFFER_BINDING, &FBid);
	cerr<<desc.fWidth<<'X'<<desc.fHeight<<",hand="<<FBid<<endl;
	desc.fRenderTargetHandle = FBid;
//SkSafeUnref(fCurRenderTarget);
	fCurRenderTarget = fCurContext->wrapBackendRenderTarget(desc);
	//fCurContext->setRenderTarget(fCurRenderTarget);
	//	detach();

	//load view
	//SkView::F2BIter iter(this);
	//SkView* prev = iter.next();
	//if (prev) {
	//	cout<<"get prev\n"<<endl;
	//	prev->detachFromParent();
	//}

	//detach();	
}

SkCanvas* ClvWindow::createCanvas() {
	cout<<__FUNCTION__<<endl;	//return INHERITED::createCanvas();
	SkAutoTUnref<SkDevice> device{new SkGpuDevice{fCurContext, fCurRenderTarget}};
		cerr<<"try SkDevice"<<endl;
	return new SkCanvas{device};
}

void ClvWindow::draw(SkCanvas* canvas){
	cout<<__FUNCTION__<<endl;
	INHERITED::draw(canvas);

	fCurContext->flush();
	fCurContext->setRenderTarget(fCurRenderTarget);
	/**
	const SkBitmap& bm = getBitmap();
	fCurRenderTarget->writePixels(0, 0,
		bm.width(), bm.height(),
		kSkia8888_GrPixelConfig,
		bm.getPixels(),
		bm.rowBytes());
	*/
	present();
}

void ClvWindow::onSizeChange(){
	INHERITED::onSizeChange();
	cout<<__FUNCTION__<<endl;
}

bool ClvWindow::onEvent(const SkEvent& evt){
	//SkString typeE;
	//evt.getType(&typeE);
	//cout<<__FUNCTION__<<':'<<typeE.c_str()<<endl;
	//if(evt.isType(gUpdateWindowTitleEvtName)){
	//	updateTitle();
	//	return true;
	//}
	return INHERITED::onEvent(evt);
}

bool ClvWindow::onClick(Click* click){
	cout<<__FUNCTION__<<endl;
	return true;
}
/*
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
  glBegin(GL_POINTS);
  glVertex3f(0,10,0);
  glVertex3f(10,10,0);
  glVertex3f(0,0,0);
  glEnd();
*/
void ClvWindow::onDraw(SkCanvas* canvas){
	canvas->drawColor(0xFFFF8080);
	cout<<__FUNCTION__<<fCurRenderTarget->numSamples()<<endl;
}

bool ClvWindow::onHandleKey(SkKey key) {
	cout<<__FUNCTION__<<endl;
	switch (key) {
	case kRight_SkKey:
		break;
	case kLeft_SkKey:
		return true;
	case kUp_SkKey:
		return true;
	case kDown_SkKey:
		return true;
	case kOK_SkKey:
		return true;
	case kBack_SkKey:
		return true;
	default:
		break;
	}
	return INHERITED::onHandleKey(key);
}


void ClvView::onDraw(SkCanvas* canvas){
	cout<<__FUNCTION__<<endl;
}

void application_init() {
    SkGraphics::Init();
    SkEvent::Init();
}

void application_term() {
    SkEvent::Term();
    SkGraphics::Term();
}

SkOSWindow* create_sk_window(void* hwnd, int argc, char** argv) {
	return new ClvWindow{hwnd};
	//return new SkOSWindow{nullptr};
}

