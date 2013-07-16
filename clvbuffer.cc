#include <gtkmm.h>
#include "clvbuffer.h"

ClvBuffer::ClvBuffer(){
	auto tag = create_tag("normal");
	tag->property_family() = "Source Code Pro";
	create_tag("keyword");
	create_tag("var:builtin");
	create_tag("ref");
	create_tag("ptr");
	create_tag("var:composite");
	create_tag("operator");
}