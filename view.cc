#define linux
#define NDEBUG
#include "SkData.h"
#include "SkCanvas.h"
#include "SkDevice.h"
#include "SkGraphics.h"
#include "SkString.h"
#include "SkTouchGesture.h"
#include "SkWindow.h"
#include "SkTypeface.h"

#include "gl/GrGLInterface.h"
#include "gl/GrGLUtil.h"
#include "GrRenderTarget.h"
#include "GrContext.h"
#include "SkGpuDevice.h"

//#include "filemap.cc"
#include <memory>
#include <iostream>
#include <iterator>
#include "filemap.h"
#include "view.h"
using namespace std;
static void inline postEventToSink(SkEvent* evt, SkEventSink* sink) {
	evt->setTargetID(sink->getSinkID())->post();
}

ClvWindow::ClvWindow(void* hwnd,const char* filepath):INHERITED{hwnd},hwnd{hwnd},file{filepath},fBorder{12}{
	//cout<<__PRETTY_FUNCTION__<<endl;
	//setConfig(SkBitmap::kRGB_565_Config);
	setConfig(SkBitmap::kARGB_8888_Config);
	setVisibleP(true);
	setClipToBounds(false);
	attachChildToFront(new ClvScroller{ClvScroller::ScrollerType::ScrollX})->unref();
	attachChildToFront(new ClvScroller{ClvScroller::ScrollerType::ScrollY})->unref();
	resetChildrenView();
	setFormat();

	fMSAASampleCount=0;
	//FIXME:if use MSAA in attach XGL;crash at ClvWindow::onSizeChang
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

	GrBackendRenderTargetDesc desc;
	desc.fWidth = SkScalarRound(width());
	desc.fHeight = SkScalarRound(height());

	desc.fConfig = kSkia8888_GrPixelConfig;
	desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
	desc.fSampleCnt = attachmentInfo.fSampleCount;
	desc.fStencilBits = attachmentInfo.fStencilBits;
	GrGLint FBid;
	GR_GL_GetIntegerv(fCurIntf, GR_GL_FRAMEBUFFER_BINDING, &FBid);
	//cerr<<desc.fWidth<<'X'<<desc.fHeight<<",hand="<<FBid<<endl;
	desc.fRenderTargetHandle = FBid;
	//SkSafeUnref(fCurRenderTarget);
	fCurRenderTarget = fCurContext->wrapBackendRenderTarget(desc);

	//load view
	//SkView::F2BIter iter(this);
	//SkView* prev = iter.next();
	//if (prev) {
	//	cout<<"get prev\n"<<endl;
	//	prev->detachFromParent();
	//}	
}

void ClvWindow::resetChildrenView(){
	SkView::F2BIter iter(this);
	auto ScrollX = iter.next();
	auto ScrollY = iter.next();
	ScrollX->setLocY(Eheight());
	ScrollX->setSize(Ewidth(),fBorder);
	ScrollY->setLocX(Ewidth());
	ScrollY->setSize(fBorder,Eheight());
};

bool ClvWindow::onHandleChar(SkUnichar uni) {//input
	cout<<__PRETTY_FUNCTION__<<uni<<endl;
	int dx = 0xFF;
	int dy = 0xFF;
	switch (uni) {
	case '5': dx =  0; dy =  0; break;
	case '8': dx =  0; dy = -1; break;
	case '6': dx =  1; dy =  0; break;
	case '2': dx =  0; dy =  1; break;
	case '4': dx = -1; dy =  0; break;
	case '7': dx = -1; dy = -1; break;
	case '9': dx =  1; dy = -1; break;
	case '3': dx =  1; dy =  1; break;
	case '1': dx = -1; dy =  1; break;
	//default: break;
	}
	
	return INHERITED::onHandleChar(uni);
}

SkView::Click* ClvWindow::onFindClickHandler(SkScalar x, SkScalar y,unsigned modi) {
	cout<<__PRETTY_FUNCTION__<<x<<'.'<<y<<endl;
	return INHERITED::onFindClickHandler(x,y,modi);
}

bool ClvWindow::onSendClickToChildren(SkScalar x, SkScalar y, unsigned modi) {
	cout<<__PRETTY_FUNCTION__<<x<<'.'<<y<<endl;
	return INHERITED::onSendClickToChildren(x,y,modi);
}

bool ClvWindow::onClick(Click* click) {
	float x = static_cast<float>(click->fICurr.fX);
	float y = static_cast<float>(click->fICurr.fY);
	cout<<__PRETTY_FUNCTION__<<x<<'.'<<y<<endl;
	return true;
}

bool ClvWindow::setFormat(){
	fBGColor = 0xFF888888;
	fTypeface = SkTypeface::CreateFromName("Source Code Pro Light", SkTypeface::kNormal);
	paint.setTypeface(fTypeface);
	SkScalar textSize = SkIntToScalar(15);
	paint.setAntiAlias(true);
	paint.setLCDRenderText(true);
	paint.setTextSize(textSize);
	paint.setColor(0xFF00FFFF);
	return true;
}

SkCanvas* ClvWindow::createCanvas() {
	//cout<<__PRETTY_FUNCTION__<<endl;return INHERITED::createCanvas();
	SkAutoTUnref<SkDevice> device{new SkGpuDevice{fCurContext, fCurRenderTarget}};
	return new SkCanvas{device};
}

void ClvWindow::draw(SkCanvas* canvas){
	//cout<<__PRETTY_FUNCTION__<<endl;
	//setFormat();
	INHERITED::draw(canvas);
	fCurContext->flush();
	/** Raster bits into GPU RenderTarget
	fCurContext->setRenderTarget(fCurRenderTarget);
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
	//cout<<__PRETTY_FUNCTION__<<width()<<'X'<<height()<<endl;
	INHERITED::onSizeChange();
	resetChildrenView();

	//detach();
	AttachmentInfo attachmentInfo;
	bool result = attach(kNativeGL_BackEndType, fMSAASampleCount, &attachmentInfo);
	if(!result)
		cerr<<"3D Attach err"<<endl;

	GrBackendRenderTargetDesc desc;
	desc.fWidth = SkScalarRound(width());
	desc.fHeight = SkScalarRound(height());
	desc.fConfig = kSkia8888_GrPixelConfig;
	desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
	desc.fSampleCnt = attachmentInfo.fSampleCount;
	desc.fStencilBits = attachmentInfo.fStencilBits;
	GrGLint FBid;
	GR_GL_GetIntegerv(fCurIntf, GR_GL_FRAMEBUFFER_BINDING, &FBid);
	desc.fRenderTargetHandle = FBid;
	SkSafeUnref(fCurRenderTarget);
	fCurRenderTarget = fCurContext->wrapBackendRenderTarget(desc);
}

bool ClvWindow::onEvent(const SkEvent& evt){
	//SkString typeE;
	//evt.getType(&typeE);
	//cout<<__PRETTY_FUNCTION__<<':'<<typeE.c_str()<<endl;
	//if(evt.isType(gUpdateWindowTitleEvtName)){
	//	updateTitle();
	//	return true;
	//}
	return INHERITED::onEvent(evt);
}


void ClvWindow::onDraw(SkCanvas* canvas){
	cout<<__PRETTY_FUNCTION__<<endl;
	canvas->drawColor(fBGColor);

	SkScalar x0 = SkIntToScalar(30);
	SkScalar y = SkIntToScalar(20);
	canvas->drawText(file.Get(),file.Length(), x0, y, paint);

	auto itr = file.begin();
	y += paint.getFontSpacing();
	canvas->drawText(itr->Get(),itr->Length(), x0, y, paint);
	cout<<"Length"<<itr->Length()<<endl;
	++itr;
	y += paint.getFontSpacing();
	canvas->drawText(itr->Get(),itr->Length(), x0, y, paint);
	cout<<"Length"<<itr->Length()<<endl;
}

bool ClvWindow::onHandleKey(SkKey key){
	//cout<<__PRETTY_FUNCTION__<<endl;
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

bool ClvWindow::onHandleKeyUp(SkKey key){
	return INHERITED::onHandleKeyUp(key);
}

ClvScroller::ClvScroller(ScrollerType st):fType{st},fBorder{1}{
	//cout<<__PRETTY_FUNCTION__<<endl;
	setVisibleP(true);
	setClipToBounds(false);
	paint.setAntiAlias(true);
	paint.setColor(0xAAFFFFFF);
}

void ClvScroller::onDraw(SkCanvas* canvas){
	//cout<<__PRETTY_FUNCTION__<<endl<<width()<<'X'<<height()<<':'<<locX()<<'X'<<locY()<<endl;
	//canvas->drawColor(0x55FFFFFF);
	if(fType==ScrollY){
		auto rect = SkRect::MakeLTRB(0,fBorder,width(),height()-2*fBorder);
		canvas->drawRoundRect(rect,3,3,paint);
	}else{
		auto rect = SkRect::MakeLTRB(fBorder,0,width()-2*fBorder,height());
		canvas->drawRoundRect(rect,3,3,paint);
	}

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
	if(argc!=2)
		return new ClvWindow{hwnd,nullptr};
	return new ClvWindow{hwnd,argv[1]};
}

