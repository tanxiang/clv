#include "SkCanvas.h"
#include "SkGraphics.h"
#include "SkString.h"
#include "view.h"
#include "SkWindow.h"

//DEFINE_string(o, "skhello.png", "The filename to write the image.");
//DEFINE_string(t, "Hello", "The string to write.");
void application_init() {
    SkGraphics::Init();
    SkEvent::Init();
}

void application_term() {
    SkEvent::Term();
    SkGraphics::Term();
}

SkOSWindow* create_sk_window(void* hwnd, int argc, char** argv) {
    return nullptr;
}

int main() {

    SkAutoGraphics ag;
    //SkString path("skhello.png");
    SkString text("Hello");

    //if (!FLAGS_o.isEmpty()) {
    //    path.set(FLAGS_o[0]);
    //}
    //if (!FLAGS_t.isEmpty()) {
    //    text.set(FLAGS_t[0]);
    //}

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setTextSize(SkIntToScalar(30));
    SkScalar width = paint.measureText(text.c_str(), text.size());
    SkScalar spacing = paint.getFontSpacing();

    int w = SkScalarRound(width) + 30;
    int h = SkScalarRound(spacing) + 30;
    SkBitmap bitmap;
    bitmap.setConfig(SkBitmap::kARGB_8888_Config, w, h);
    bitmap.allocPixels();

    SkCanvas canvas(bitmap);
    canvas.drawColor(SK_ColorWHITE);

    paint.setTextAlign(SkPaint::kCenter_Align);
    canvas.drawText(text.c_str(), text.size(),
                    SkIntToScalar(w)/2, SkIntToScalar(h)*2/3,
                    paint);

    //bool success = SkImageEncoder::EncodeFile(path.c_str(), bitmap,SkImageEncoder::kPNG_Type, 100);
    //if (!success) {
    //    SkDebugf("--- failed to write %s\n", path.c_str());
    //}
    return 0;
}
