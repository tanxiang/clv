#include "clvbuffer.h"
#include "clvparser.h"

ClvBuffer::ClvBuffer(){
	auto tag = create_tag("normal");
	tag->property_family() = "Source Code Pro";
	//tag->property_background () = "green";
	tag->property_foreground_gdk() = Gdk::Color{"red"};

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
#include <iostream>
void ClvBuffer::on_insert(const TextBuffer::iterator& pos, const Glib::ustring& text, int bytes){
	TextBuffer::on_insert(pos,text,bytes);
	if(bytes==1){
		switch(text[0]){
		case '>':
			if((----TextBuffer::iterator{pos}).get_char()=='-') 
				std::cout<<"complete ->\n";
			break;
		case ':':
			if((----TextBuffer::iterator{pos}).get_char()==':') 
				std::cout<<"complete ::\n";
			break;
		case '.':
			std::cout<<"complete .\n";
		}
	}
} 
