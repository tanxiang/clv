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
    return new SkOSWindow{nullptr};
}

