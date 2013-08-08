#include "clvtkmm.h"
#include "clvbuffer.h"
#include <pangomm.h>
#include <iostream>


ClvFViewBox::ClvFViewBox(unorder_tree<line> &file_ref):
	file(file_ref),fileview(file){
	//set_homogeneous(false);
	//lineview.queue_draw_area(0,0,30,30);
	//FIXME:reconfig w,h by font w,h
	//lineview.set_size_request(10);
	pack_start(lineview,Gtk::PACK_SHRINK);

	pack_start(scrolledview);
	scrolledview.add(fileview);
	thumview.set_size_request(100);
	pack_start(thumview,Gtk::PACK_SHRINK);
}

void ClvFViewBox::save(){
	//std::cout<<"save:"<<reinterpret_cast<unsigned int>(this)<<std::endl;
	std::cout<<"save:"<<this<<std::endl;
}

void ClvFViewBox::close(){
	std::cout<<"close:"<<this<<std::endl;
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
#include <iterator>

ClvFileBox::ClvFileBox(Glib::ustring fs):Gtk::Box(Gtk::ORIENTATION_VERTICAL,2),
	file_name(fs),file_stream(file_name),
	file_context(std::istream_iterator<line>{file_stream},std::istream_iterator<line>{}),
	view(file_context),tab_label(file_name){
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
	css->load_from_data(button_style);
	//style->add_provider(css, 0);
	
	//style->set_path(bt_save.get_path());
	bt_save.set_relief(Gtk::RELIEF_NONE);
	save_icon.set_from_icon_name("gtk-save",Gtk::ICON_SIZE_MENU);
	bt_save.set_image (save_icon);
	bt_save.get_style_context()->add_provider(css,400);
	bt_save.signal_clicked().connect(sigc::mem_fun(view,&ClvFViewBox::save));

	bt_close.set_relief(Gtk::RELIEF_NONE);
	close_icon.set_from_icon_name("window-close-symbolic",Gtk::ICON_SIZE_MENU);
	bt_close.set_image (close_icon);
	bt_close.get_style_context()->add_provider(css,400);
	bt_close.signal_clicked().connect(sigc::mem_fun(view,&ClvFViewBox::close));

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

ClvNotebook::ClvNotebook(std::vector<std::string> &fn){
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
