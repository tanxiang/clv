#define linux
#define NDEBUG
#include "view.h"
#include "SkData.h"
#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkString.h"
#include "SkTouchGesture.h"
#include "SkWindow.h"

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

#include <iostream>
using namespace std;

ClvWindow::ClvWindow(void* hwnd):SkOSWindow{hwnd},hwnd(hwnd){
	cout<<__FUNCTION__<<endl;
	//register canvas here?
	
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
	cout<<__FUNCTION__<<typeE.c_str()<<endl;
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
    return this->INHERITED::onHandleKey(key);
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

