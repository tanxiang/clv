#include "clvtkmm.h"
#include "clvbuffer.h"
#include <pangomm.h>
#include "dbg.h"


ClvFViewBox::ClvFViewBox(context<line> &file_ref):
	Glib::ObjectBase(typeid(ClvFViewBox)),
	Gtk::Scrollable(),
	file(file_ref),edit_view(file)
{
	set_has_window(true);
	//add_events(Gdk::BUTTON_PRESS_MASK|Gdk::BUTTON_RELEASE_MASK|Gdk::SCROLL_MASK|Gdk::TOUCH_MASK|Gdk::SMOOTH_SCROLL_MASK);
}

void ClvFViewBox::on_realize(){
	GdkWindowAttr attributes;
	//Gtk::Container::on_realize();
	auto allocation = get_allocation();
	attributes.x = allocation.get_x();
	attributes.y = allocation.get_y();
	attributes.width = allocation.get_width();
	attributes.height = allocation.get_height();
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = get_visual()->gobj();
	attributes.event_mask = get_events() | GDK_VISIBILITY_NOTIFY_MASK | GDK_EXPOSURE_MASK;
	int attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_SCROLL_MASK | GDK_TOUCH_MASK | GDK_SMOOTH_SCROLL_MASK;;
	auto window = Gdk::Window::create(get_parent_window(), &attributes, Gdk::WA_X | Gdk::WA_Y );
	set_window(window);
	register_window(window);
	//line_view.set_parent_window(window);
	edit_view.set_parent_window(window);
	edit_view.set_parent(*this);
	//debug<<"get_window:"<<get_window().operator->()<<std::endl;
	get_hadjustment()->configure(0,0,1200,1,10,100);
	get_vadjustment()->configure(0,0,2400,1,10,100);

	get_hadjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&ClvFViewBox::on_hadjustment));
	get_vadjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&ClvFViewBox::on_vadjustment));

	//add(line_view);
	//add(edit_view);
	set_realized(true);
	if(get_has_window())
		debug<<"get_has_window"<<std::endl;
	
	//show();
	//edit_view.map();
	//show_all_children();
}

void ClvFViewBox::on_unrealize(){
	//unregister_window(get_window());
	Gtk::Container::on_unrealize();
}

void ClvFViewBox::on_size_allocate(Gtk::Allocation& allocation){
	debug<<__PRETTY_FUNCTION__<<std::endl;
	//set_allocation(allocation);
	Gtk::Container::on_size_allocate(allocation);
	if(true){
		debug<<"edit_view realized"<<std::endl;
		Gtk::Allocation edit_view_allocation{-get_hadjustment()->get_value(),-get_vadjustment()->get_value(),
			get_hadjustment()->get_upper(),get_vadjustment()->get_upper()};
		edit_view.size_allocate(edit_view_allocation);
	}
	if(line_view.get_realized()){
		debug<<"line_view realized"<<std::endl;
	}
}

bool ClvFViewBox::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	debug<<__PRETTY_FUNCTION__<<std::endl;
	if(should_draw_window(cr,get_window()))
		debug<<"should_draw_window"<<std::endl;
	return true;
}

void ClvFViewBox::on_hadjustment(){
	//debug<<"hadjustment:"<<std::endl;
	if(edit_view.get_realized()){
		int x,y;
		edit_view.get_window()->get_position(x,y);
		int new_x = - get_hadjustment()->get_value();
		edit_view.get_window()->move(new_x,y);
	}
	//if(line_view.get_realized())
	//	line_view.get_window()->move(new_x,y);
}

void ClvFViewBox::on_vadjustment(){
	if(edit_view.get_realized()){
		int x,y;
		edit_view.get_window()->get_position(x,y);
		int new_y = - get_vadjustment()->get_value();
		edit_view.get_window()->move(x,new_y);
	}
}

void ClvFViewBox::save(){
	//std::cout<<"save:"<<reinterpret_cast<unsigned int>(this)<<std::endl;
	debug<<"save:"<<this<<std::endl;
}

void ClvFViewBox::close(){
	debug<<"close:"<<this<<std::endl;
}



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


#include <iterator>

ClvPageBox::ClvPageBox(Glib::ustring fs):Gtk::Box(Gtk::ORIENTATION_VERTICAL,2),
	file_name(fs),file_stream(file_name),
	file_context(std::istream_iterator<line>{file_stream},std::istream_iterator<line>{}),
	content_view(file_context),tab_label(file_name){
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
	bt_save.signal_clicked().connect(sigc::mem_fun(content_view,&ClvFViewBox::save));

	bt_close.set_relief(Gtk::RELIEF_NONE);
	close_icon.set_from_icon_name("window-close-symbolic",Gtk::ICON_SIZE_MENU);
	bt_close.set_image (close_icon);
	bt_close.get_style_context()->add_provider(css,400);
	bt_close.signal_clicked().connect(sigc::mem_fun(content_view,&ClvFViewBox::close));

	//auto style = tab_box.get_style_context();
	//style->add_provider(css,0);
	//style->add_class(GTK_STYLE_CLASS_RAISED);
	//style->add_class(GTK_STYLE_CLASS_LINKED);
	tab_box.set_spacing(0);
	tab_box.pack_start(tab_label,true, true, 0);
	tab_box.pack_start(bt_save,false,false,0);
	tab_box.pack_start(bt_close,false,false,0);
	tab_box.show_all_children();

}

void ClvPageBox::on_realize(){
	//pack_start(head_view,Gtk::PACK_SHRINK);
	//head_view.set_size_request(0,0);
	pack_start(main_view);
	main_view.pack_start(scrolled_content_view);
	scrolled_content_view.add(content_view);
	scrolled_content_view.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
	thumb_view.set_size_request(10);
	main_view.pack_start(thumb_view,Gtk::PACK_SHRINK);
	pack_end(tool_bar,Gtk::PACK_SHRINK);
	Gtk::Box::on_realize();
	show_all_children();
}

void ClvPageBox::on_unrealize(){
	Gtk::Box::on_unrealize();
}

void ClvNotebook::on_page_switch ( Gtk::Widget *page, int page_num ){
	debug<<"sw page:"<<page<<"\tnum:"<<page_num<<"\n";
	if(current_page)
		current_page->set_activates(false);
	current_page = static_cast<ClvPageBox*>(page);
	current_page->set_activates(true);
}

ClvNotebook::ClvNotebook(std::vector<std::string> &fn):current_page{nullptr}{
	set_scrollable();
	if(fn.empty()){
		f_boxs.push_back(std::unique_ptr<ClvPageBox>{new ClvPageBox{}});
		prepend_page(*f_boxs[0], "*UnSaved");
	}
	else{
		for(auto &fname : fn){
			f_boxs.push_back(std::unique_ptr<ClvPageBox>{new ClvPageBox{fname}});
			//prepend_page(*f_boxs.back(),fname.c_str());
			prepend_page(*f_boxs.back(),f_boxs.back()->get_tab_box());
		}
	}
	signal_switch_page().connect(sigc::mem_fun(*this,&ClvNotebook::on_page_switch));
}


bool ClvtkWindow::on_key_release_event(GdkEventKey* event){
	//std::cout<<"ClvtkWindow\n";
	return false;
}

ClvtkWindow::ClvtkWindow(std::vector<std::string> fn):flist_notebook(fn){
	set_size_request(500,400);
	add(flist_notebook);
	show_all_children();
};
