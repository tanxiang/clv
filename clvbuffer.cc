#include <gtkmm.h>
#include "clvbuffer.h"

ClvBuffer::ClvBuffer(){
	auto tag = create_tag("normal");
	tag->property_family() = "Source Code Pro";
	//tag->property_background () = "green";
	tag->property_foreground_gdk() = Gdk::Color{"white"};

	tag = create_tag("keyword");
	tag->property_weight() = PANGO_WEIGHT_BOLD;
	tag->property_foreground_gdk() = Gdk::Color{"green"};

	tag = create_tag("var:builtin");
	tag->property_foreground_gdk() = Gdk::Color{"blue"};

	tag = create_tag("var:ptr");
	tag->property_foreground_gdk() = Gdk::Color{"blue"};

	tag = create_tag("var:composite");
	tag->property_foreground_gdk() = Gdk::Color{"purple"};

	tag = create_tag("var:instantiation");
	tag->property_foreground_gdk() = Gdk::Color{"orange"};

	tag = create_tag("ref");
	tag->property_foreground_gdk() = Gdk::Color{"blue"};

	tag = create_tag("operator");
}