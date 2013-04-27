
class ClvScroller : public SkView {
public:
	enum ScrollerType{
		ScrollX,ScrollY
	};
	ClvScroller(ScrollerType st);
	virtual ~ClvScroller(){}

protected:
	void onDrawBackground(SkCanvas*);
	//virtual void draw(SkCanvas*) override;
	virtual void onDraw(SkCanvas*) override;

private:
	int fBorder;
	SkPaint paint;
	ScrollerType fType;
	typedef SkView INHERITED;
};

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

	ClvWindow(void* hwnd,const char* filepath);
	virtual ~ClvWindow(){detach();};

	virtual SkCanvas* createCanvas() override;
	virtual void draw(SkCanvas* canvas) override;

	void setDeviceType(DeviceType type);
	DeviceType getDeviceType() const { return fDeviceType; }
	bool setFormat();
	int Ewidth(){
		return width()-fBorder;
	}
	int Eheight(){
		return height()-fBorder;
	}
protected:
	virtual bool onHandleKey(SkKey key) override;
	virtual bool onHandleKeyUp(SkKey key) override;
	virtual bool onHandleChar(SkUnichar uni) override;
	virtual Click* onFindClickHandler(SkScalar x, SkScalar y,unsigned modi) override;
	virtual bool onSendClickToChildren(SkScalar x, SkScalar y, unsigned modi) override;
	virtual bool onClick(Click* click) override;
	virtual void onDraw(SkCanvas* canvas) override;
	virtual void onSizeChange() override;
	void resetChildrenView();
	virtual bool onEvent(const SkEvent& evt) override;
private:
	DeviceType fDeviceType;

	SkTouchGesture fGesture;
	//SkScalar fZoomLevel;
	//SkScalar fZoomScale;
	//SkScrollBarView ScrollBarX,* ScrollBarY;
	int fScrollX, fScrollY;
	int fBorder;
	GrContext*              fCurContext;
	const GrGLInterface*    fCurIntf;
	GrRenderTarget*         fCurRenderTarget;
	SkTypeface*		fTypeface;
	int fMSAASampleCount;
	SkPaint paint;
	SkColor fBGColor;
	FileMap<MBLineRef<char> > file;
	typedef SkOSWindow INHERITED;
};

