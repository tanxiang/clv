#include "clvtkmm.h"
#include <pangomm.h>
#include <iostream>

//Gtk::Image image_close{Gtk::Stock::CLOSE,Gtk::IconSize::MENU};

void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
                            int width, int height){
 	//FIXME color conf
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->rectangle(0, 0, width, height);
	cr->fill();
}

bool ClvLineArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	draw_rectangle(cr, width, height);
	return true;
}

ClvFileArea::ClvFileArea(FileMap<MBLineRef<CharRef> > &file_ref):file(file_ref){
	//std::cout<<static_cast<char*>(file.Get());
	get_buffer()->set_text(static_cast<char*>(file.Get()));
}
/*
bool ClvFileArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	draw_rectangle(cr, width, height);
	draw_text(cr, width, height);
	return true;
}

void ClvFileArea::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
                       int rectangle_width, int rectangle_height){
	//FIXME need conf
	cr->set_source_rgb(1.0, 1.0, 1.0);
	Pango::FontDescription font;
	font.set_family("Source Code Pro");
	font.set_weight(Pango::WEIGHT_NORMAL);
	//for(auto &Line : file){
		//std::cout<<"asdasd";
	//}
	auto pgcontext = get_pango_context();
	auto layout = create_pango_layout(static_cast<char*>(file.Get()));
	layout->set_font_description(font);
	Pango::TabArray tab{4};
	layout->set_tabs(tab);
	//auto layoutiter = layout->get_iter();
	int nline = layout->get_line_count();
	int i=0,x=0;
	while(i<nline){
		auto line = layout->get_line(i++);
		cr->move_to(0,i*16);
		line->show_in_cairo_context(cr);
	}

	//int text_width, text_height;
	//get the text dimensions (it updates the variables -- by reference)
	//layout->get_pixel_size(text_width, text_height);

	// Position the text in the middle
	//cr->move_to((rectangle_width-text_width)/2, (rectangle_height-text_height)/2);
}
*/
bool ClvThumArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	draw_rectangle(cr, width, height);
	return true;
}

ClvFViewBox::ClvFViewBox(FileMap<MBLineRef<CharRef> > &file_ref):
	file(file_ref),fileview(file){
	//set_homogeneous(false);
	//lineview.queue_draw_area(0,0,30,30);
	//FIXME:reconfig w,h by font w,h
	lineview.set_size_request(50);
	pack_start(lineview,Gtk::PACK_SHRINK);

	pack_start(scrolledview);
	scrolledview.add(fileview);
	thumview.set_size_request(100);
	pack_start(thumview,Gtk::PACK_SHRINK);
}
/*
ClvFViewBox::~ClvFViewBox(){
	std::cout<<"free vbox"<<std::endl;
}*/

ClvToolBox::ClvToolBox():file_mode("c++",true),
	search("search",true),options("options",true){
	auto style = get_style_context();
	style->add_class(GTK_STYLE_CLASS_RAISED);
	style->add_class(GTK_STYLE_CLASS_LINKED);
	pack_start(search,Gtk::PACK_SHRINK);
	pack_start(file_mode,Gtk::PACK_SHRINK);
	pack_start(tab_size,Gtk::PACK_SHRINK);
	pack_start(options,Gtk::PACK_SHRINK);
}
/*
Gtk::Image ClvFileBox::close_icon=[]{
		Gtk::Image img;
		img.set_from_icon_name("window-close-symbolic",Gtk::ICON_SIZE_MENU);
		return img;
	}
*/

//void ClvFileBox::setup_icons(){
	//close_icon.set_from_icon_name("window-close-symbolic",Gtk::ICON_SIZE_MENU);
//}

ClvFileBox::ClvFileBox(Glib::ustring fs):Gtk::Box(Gtk::ORIENTATION_VERTICAL,2),
	filename(fs),file(filename.c_str()),view(file),tab_label(filename){
	static const gchar button_style[] =
		"* {\n"
		  "-GtkButton-default-border : 0;\n"
		  "-GtkButton-default-outside-border : 0;\n"
		  "-GtkButton-inner-border: 0;\n"
		  "-GtkWidget-focus-line-width : 0;\n"
		  "-GtkWidget-focus-padding : 0;\n"
		  "padding: 0;\n"
		"}";
	auto css = Gtk::CssProvider::create();
	//auto style = Gtk::StyleContext::create();
	css->load_from_data( button_style);
	//style->add_provider(css, 0);
	
	//style->set_path(bt_save.get_path());
	bt_save.set_relief(Gtk::RELIEF_NONE);
	save_icon.set_from_icon_name("gtk-save",Gtk::ICON_SIZE_MENU);
	bt_save.set_image (save_icon);
	bt_save.get_style_context()->add_provider(css,400);

	bt_close.set_relief(Gtk::RELIEF_NONE);
	close_icon.set_from_icon_name("window-close-symbolic",Gtk::ICON_SIZE_MENU);
	bt_close.set_image (close_icon);
	bt_close.get_style_context()->add_provider(css,400);
	//auto style = tab_box.get_style_context();
	//style->add_provider(css,0);
	//style->add_class(GTK_STYLE_CLASS_RAISED);
	//style->add_class(GTK_STYLE_CLASS_LINKED);
	tab_box.set_spacing(0);
	tab_box.pack_start(tab_label,true, true, 0);
	tab_box.pack_start(bt_save,false,false,0);
	tab_box.pack_start(bt_close,false,false,0);
	tab_box.show_all_children();
	pack_start(view);
	pack_end(tool_bar,Gtk::PACK_SHRINK);
}

ClvNotebook::ClvNotebook(std::vector<std::string> fn){
	if(fn.empty()){
		f_boxs.push_back(std::unique_ptr<ClvFileBox>{new ClvFileBox{}});
		prepend_page(*f_boxs[0], "*UnSaved");
	}
	else{
		for(auto &fname : fn){
			f_boxs.push_back(std::unique_ptr<ClvFileBox>{new ClvFileBox{fname}});
			//prepend_page(*f_boxs.back(),fname.c_str());
			prepend_page(*f_boxs.back(),f_boxs.back()->get_tab_box());
		}
	}
}

ClvtkWindow::ClvtkWindow(std::vector<std::string> fn):flist_notebook(fn){
	set_size_request(500,400);
	add(flist_notebook);
	//f_boxs.push_back(std::unique_ptr<ClvFileBox>{new ClvFileBox{}});
	//flist_notebook.prepend_page(*f_boxs[0], "*UnSaved");
	//main_box.pack_end(tool_box,Gtk::PACK_SHRINK);
	//main_box.set_spacing(3);
	show_all_children();
};
