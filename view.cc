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
	virtual ~ClvWindow(){};

	virtual SkCanvas* createCanvas() override;

	virtual void draw(SkCanvas* canvas) override;

	void setDeviceType(DeviceType type);
	DeviceType getDeviceType() const { return fDeviceType; }

protected:
	virtual bool onHandleKey(SkKey key) override;
	virtual void onDraw(SkCanvas* canvas) override;
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
	int fMSAASampleCount;

	typedef SkOSWindow INHERITED;
};

class ClvView : public SkView {
public:
	ClvView():fBGColor(SK_ColorWHITE){
	}

	virtual ~ClvView(){}

protected:
	void onDrawBackground(SkCanvas*);
	virtual void draw(SkCanvas*) override;
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

	AttachmentInfo attachmentInfo;
	bool result = attach(kNativeGL_BackEndType, 2, &attachmentInfo);
	if(!result){
		cerr<<"3D Attch err"<<endl;
		//canvas = INHERITED::createCanvas();
		//return canvas;
	}
		cerr<<"try 3D Attch"<<endl;
	fCurIntf = GrGLCreateNativeInterface();
		cerr<<"try 3D Attch"<<endl;
	fCurContext = GrContext::Create(kOpenGL_GrBackend, (GrBackendContext) fCurIntf);
		cerr<<"try 3D Create"<<endl;
	if (NULL == fCurContext || NULL == fCurIntf) {
		cerr<<"3D Create err"<<endl;
		detach();
	}
	GrBackendRenderTargetDesc desc;
	desc.fWidth = SkScalarRound(width());
	desc.fHeight = SkScalarRound(height());
	desc.fConfig = kSkia8888_GrPixelConfig;
	desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
	desc.fSampleCnt = attachmentInfo.fSampleCount;
	desc.fStencilBits = attachmentInfo.fStencilBits;
	GrGLint buffer;
	GR_GL_GetIntegerv(fCurIntf, GR_GL_FRAMEBUFFER_BINDING, &buffer);
	desc.fRenderTargetHandle = buffer;
	fCurRenderTarget = fCurContext->wrapBackendRenderTarget(desc);
	fCurContext->setRenderTarget(fCurRenderTarget);
	//auto fTypeface = SkTypeface::CreateFromTypeface(NULL, SkTypeface::kBold);
	//load view
	//SkView::F2BIter iter(this);
	//SkView* prev = iter.next();
	//if (prev) {
	//	cout<<"get prev\n"<<endl;
	//	prev->detachFromParent();
	//}

	//setConfig(SkBitmap::kRGB_565_Config);
	setConfig(SkBitmap::kARGB_8888_Config);
	setVisibleP(true);
	//setClipToBounds(false);
	//attachChildToFront(new ClvView{})->unref();	
}

SkCanvas* ClvWindow::createCanvas() {
	cout<<__FUNCTION__<<endl;	//return INHERITED::createCanvas();
	SkAutoTUnref<SkDevice> device{new SkGpuDevice{fCurContext, fCurRenderTarget}};
		cerr<<"try 3D dev"<<endl;

	return new SkCanvas{device};

}

void ClvWindow::draw(SkCanvas* canvas){
	cout<<__FUNCTION__<<endl;
		canvas->drawColor(0xFFFF8080);
}

bool ClvWindow::onEvent(const SkEvent& evt){
	SkString typeE;
	evt.getType(&typeE);
	cout<<__FUNCTION__<<':'<<typeE.c_str()<<endl;
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

void ClvWindow::onDraw(SkCanvas* canvas){
	cout<<__FUNCTION__<<endl;
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

void ClvView::draw(SkCanvas* canvas){
	cout<<__FUNCTION__<<endl;
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

