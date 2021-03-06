#include "clvtkmm.h"
#include "clvbuffer.h"
#include <pangomm.h>
#include "dbg.h"
namespace clv{
FViewBox::FViewBox(clv::context<clv::line> &file_ref):
	Glib::ObjectBase(typeid(FViewBox)),
	Gtk::Scrollable(),
	file(file_ref),edit_view(file),
	backing_surface_x(0),backing_surface_y(0),
	backing_surface_w(0),backing_surface_h(0)
{
	set_has_window(true);
	set_redraw_on_allocate(false);
}

void FViewBox::on_realize(){
	set_realized(true);
	GdkWindowAttr attributes;
	memset(&attributes, 0, sizeof(attributes));
	auto allocation = get_allocation();
	attributes.x = allocation.get_x();
	attributes.y = allocation.get_y();
	attributes.width = allocation.get_width();
	attributes.height = allocation.get_height();
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = get_visual()->gobj();
	attributes.event_mask = get_events() | GDK_VISIBILITY_NOTIFY_MASK | GDK_EXPOSURE_MASK;
	//int attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_SCROLL_MASK | GDK_TOUCH_MASK | GDK_SMOOTH_SCROLL_MASK;;
	auto window = Gdk::Window::create(get_parent_window(), &attributes, Gdk::WA_X | Gdk::WA_Y );
	set_window(window);
	register_window(window);

	Gdk::RGBA bg_color;
	bg_color.set_rgba(0.3,0.4,0.5);
	override_background_color(bg_color);
	//override_color(Gdk::RGBA("blue"));
	//line_view.set_parent(*this);
	edit_view.set_parent(*this);
	//debug<<"edit_view.get_parent_window:"<<edit_view.get_parent_window().operator->()<<':'<<window.operator->()<<std::endl;
	//edit_view.set_parent_window(window);
	get_hadjustment()->configure(0,0,1200,1,10,100);
	get_vadjustment()->configure(0,0,2400,1,10,100);

	get_hadjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&FViewBox::on_hadjustment));
	get_vadjustment()->signal_value_changed().connect(sigc::mem_fun(*this,&FViewBox::on_vadjustment));
}

void FViewBox::on_unrealize(){
	//unregister_window(get_window());
	set_realized(false);
	Gtk::Container::on_unrealize();
}

void FViewBox::on_size_allocate(Gtk::Allocation& allocation){
	debug<<__PRETTY_FUNCTION__<<std::endl;
	set_allocation(allocation);
	get_window()->move_resize(allocation.get_x(),allocation.get_y(),
		allocation.get_width(),allocation.get_height());
#if 0
	//get_window()->lower();
	if(edit_view.get_visible())
		debug<<"edit_view visible"<<std::endl;
	else
		debug<<"edit_view unvisible"<<std::endl;
	if(line_view.get_realized()){
		debug<<"line_view realized"<<std::endl;
	}
#endif
	if(edit_view.get_realized()){
		Gtk::Allocation edit_view_allocation{static_cast<int>(-get_hadjustment()->get_value()),
			static_cast<int>(-get_vadjustment()->get_value()),
			static_cast<int>(get_hadjustment()->get_upper()),
			static_cast<int>(get_vadjustment()->get_upper())};
		edit_view.size_allocate(edit_view_allocation);
	}
}

//GdkWindow* _gtk_cairo_get_event_window (cairo_t *cr);
/* The extra size of the offscreen surface we allocate
   to make scrolling more efficient */
#define DEFAULT_EXTRA_SIZE 64
int FViewBox::extra_width = DEFAULT_EXTRA_SIZE;
int FViewBox::extra_height = DEFAULT_EXTRA_SIZE;

bool FViewBox::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
	debug<<__PRETTY_FUNCTION__<<std::endl;
	if(should_draw_window(cr,get_window())){
		auto style = get_style_context();
		style->context_save();
		style->add_class(GTK_STYLE_CLASS_FRAME);
		style->render_frame(cr,0,0,get_window()->get_width(),get_window()->get_height());
		style->context_restore();
	}
	//static cairo_user_data_key_t event_window_key;
#if 0
	debug<<"cr user data"<<cairo_get_user_data(cr->cobj(),&event_window_key)<<':'<<get_window()->gobj()<<':'<<edit_view.get_window()->gobj()<<std::endl;
	if(should_draw_window(cr,edit_view.get_window())){
		debug<<"should_draw_window edit_view"<<std::endl;
	}
	else
#endif
	if(should_draw_window(cr,get_window())){//FIXME
		Cairo::RectangleInt view_rect{0,0,get_window()->get_width(),get_window()->get_height()};
		Cairo::RectangleInt canvas_rect{0,0,1200,2400};
		//Cairo::RectangleInt view_in_canvas_pos{-canvas_rect.x,-canvas_rect.y,view_rect.width,view_rect.height};
		auto surface_w = view_rect.width;
		auto surface_h = view_rect.height;
		if(canvas_rect.width > surface_w)
			surface_w = std::min(surface_w + extra_width,canvas_rect.width); 
		if(canvas_rect.height > surface_h)
			surface_h = std::min(surface_h + extra_height,canvas_rect.height);
		if(!backing_surface_ptr ||
			backing_surface_w < std::max(surface_w - 32, view_rect.width) || backing_surface_w > surface_w + 32||
			backing_surface_h < std::max(surface_h - 32, view_rect.height) || backing_surface_h > surface_h + 32){//or need re-build surface
			backing_surface_ptr = get_window()->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA,surface_w,surface_h);
			backing_surface_w = surface_w, backing_surface_h = surface_h;
			debug<<"fill alpha_surface_ptr:"<<backing_surface_ptr.operator->()<<std::endl;
		}
		auto backing_cr = Cairo::Context::create(backing_surface_ptr);
		auto region_dirty = Cairo::Region::create(Cairo::RectangleInt{0,0,surface_w,surface_h});
		//debug<<"back size="<<backing_surface_w<<':'<<surface_w<<std::endl;
		Gdk::Cairo::add_region_to_path(backing_cr,region_dirty);
		backing_cr->clip();
		backing_cr->set_source_rgba(0,0,0,0);
		backing_cr->set_operator(Cairo::OPERATOR_SOURCE);
		backing_cr->paint();
		backing_cr->set_operator(Cairo::OPERATOR_OVER);
		backing_cr->save();
		Gtk::Container::on_draw(backing_cr);
		backing_cr->restore();
		cr->set_operator(Cairo::OPERATOR_OVER);
		cr->set_source(backing_surface_ptr,0,0);
		cr->paint();
	}
	return true;
}

void FViewBox::forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data){
	callback((GtkWidget*)edit_view.gobj(), callback_data);
}

void FViewBox::on_hadjustment(){
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

void FViewBox::on_vadjustment(){
	if(edit_view.get_realized()){
		int x,y;
		edit_view.get_window()->get_position(x,y);
		int new_y = - get_vadjustment()->get_value();
		edit_view.get_window()->move(x,new_y);
	}
}

void FViewBox::save(){
	//std::cout<<"save:"<<reinterpret_cast<unsigned int>(this)<<std::endl;
	debug<<"save:"<<this<<std::endl;
}

void FViewBox::close(){
	debug<<"close:"<<this<<std::endl;
}



ToolBox::ToolBox():file_mode("c++",true),
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

PageBox::PageBox(Glib::ustring fs):Gtk::Box(Gtk::ORIENTATION_VERTICAL,2),
	file_name(fs),file_stream(file_name),file_io_map(file_name),
	file_context(file_io_map.begin(),file_io_map.end()),
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
	bt_save.signal_clicked().connect(sigc::mem_fun(content_view,&FViewBox::save));

	bt_close.set_relief(Gtk::RELIEF_NONE);
	close_icon.set_from_icon_name("window-close-symbolic",Gtk::ICON_SIZE_MENU);
	bt_close.set_image (close_icon);
	bt_close.get_style_context()->add_provider(css,400);
	bt_close.signal_clicked().connect(sigc::mem_fun(content_view,&FViewBox::close));

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

void PageBox::on_realize(){
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

void PageBox::on_unrealize(){
	Gtk::Box::on_unrealize();
}

void Notebook::on_page_switch ( Gtk::Widget *page, int page_num ){
	debug<<"sw page:"<<page<<"\tnum:"<<page_num<<"\n";
	if(current_page)
		current_page->set_activates(false);
	current_page = static_cast<PageBox*>(page);
	current_page->set_activates(true);
}

Notebook::Notebook(std::vector<std::string> &fn):current_page{nullptr}{
	set_scrollable();
	if(fn.empty()){
		f_boxs.push_back(std::unique_ptr<PageBox>{new PageBox{}});
		prepend_page(*f_boxs[0], "*UnSaved");
	}
	else{
		for(auto &fname : fn){
			f_boxs.push_back(std::unique_ptr<PageBox>{new PageBox{fname}});
			//prepend_page(*f_boxs.back(),fname.c_str());
			prepend_page(*f_boxs.back(),f_boxs.back()->get_tab_box());
		}
	}
	signal_switch_page().connect(sigc::mem_fun(*this,&Notebook::on_page_switch));
}


bool tkWindow::on_key_release_event(GdkEventKey* event){
	//std::cout<<"tkWindow\n";
	return false;
}

tkWindow::tkWindow(std::vector<std::string> fn):flist_notebook(fn){
	set_size_request(700,500);
	add(flist_notebook);
	show_all_children();
};

};//namespace
