#define linux
#define NDEBUG
#include "view.h"
#include "SkData.h"
#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkString.h"
#include "SkTouchGesture.h"
#include "SkWindow.h"

#include "SkTypeface.h"

//#include "gm.h"
#include <memory>

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

	//register canvas here?

	auto fTypeface = SkTypeface::CreateFromTypeface(NULL, SkTypeface::kBold);

	//load view
	SkView::F2BIter iter(this);
	SkView* prev = iter.next();
	if (prev) {
		cout<<"get prev\n"<<endl;
		prev->detachFromParent();
	}
	//attachChildToFront(new ClvView{})->unref();
	//setConfig(SkBitmap::kRGB_565_Config);
	setConfig(SkBitmap::kARGB_8888_Config);
	//loadView();
	
}

SkCanvas* ClvWindow::createCanvas() {
	cout<<__FUNCTION__<<endl;
	SkCanvas* canvas = NULL;
        //if (fDevManager) {
        //    canvas = fDevManager->createCanvas(fDeviceType, this);
        //}
        //if (NULL == canvas) {
        //    canvas = this->INHERITED::createCanvas();
        //}
	canvas = INHERITED::createCanvas();
	return canvas;
}

void ClvWindow::draw(SkCanvas* canvas){
	cout<<__FUNCTION__<<endl;
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

